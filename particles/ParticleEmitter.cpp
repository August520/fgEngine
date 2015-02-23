
namespace fg {
    namespace particles {
        ParticleAnimation::ParticleAnimation(float bornTimeMs, float lifeTimeMs, float frameTimeMs) {
            _animationFrames = nullptr;
            _bornTimeMs = bornTimeMs;
            _lifeTimeMs = lifeTimeMs;
            _frameTime = frameTimeMs * 0.001f;
            _frameCount = unsigned(lifeTimeMs / frameTimeMs) + 1;
        }

        ParticleAnimation::~ParticleAnimation() {
            delete[] _animationFrames;
        }

        void ParticleAnimation::init(const math::p3d &startPos, const math::p3d &dir, const ParticleBornParams &bornParams) {
            _startPos = startPos;
            _dir = dir;
            _animationFrames = new Frame [_frameCount];

            _dynamicParams[unsigned(ParticleParamType::VELOCITY)] = bornParams.velocity;
            _dynamicParams[unsigned(ParticleParamType::ANGLE_VELOCITY)] = bornParams.avelocity;
            _dynamicParams[unsigned(ParticleParamType::SIZE)] = bornParams.size;
            _dynamicParams[unsigned(ParticleParamType::ANGLE)] = bornParams.angle;
            _dynamicParams[unsigned(ParticleParamType::TORSION)] = bornParams.torsion;
            _dynamicParams[unsigned(ParticleParamType::STRETCH)] = 1.0f;
            _dynamicParams[unsigned(ParticleParamType::COLOR_R)] = 1.0f;
            _dynamicParams[unsigned(ParticleParamType::COLOR_G)] = 1.0f;
            _dynamicParams[unsigned(ParticleParamType::COLOR_B)] = 1.0f;
            _dynamicParams[unsigned(ParticleParamType::COLOR_A)] = 1.0f;
        }

        void ParticleAnimation::initFrames(const std::unordered_map <ParticleParamType, Modifier *> &modifiers, const math::p3d &torsionAxis) {
            math::quat torsionRotate;
            math::p3d  position;

            for(unsigned i = 0; i < _frameCount; i++) {
                float timeKoeff = float(i) / float(_frameCount);
                float localTime = timeKoeff * _lifeTimeMs * 0.001f;

                float trs = _getModifiedParticleParam(modifiers, ParticleParamType::TORSION, timeKoeff);

                position += _dir * _frameTime * _getModifiedParticleParam(modifiers, ParticleParamType::VELOCITY, timeKoeff);
                torsionRotate.rotationAxis((trs * _frameTime) / 180.0f * M_PI, torsionAxis);
                position.transform(torsionRotate);

                _animationFrames[i].position = _startPos + position;
                _animationFrames[i].size = _getModifiedParticleParam(modifiers, ParticleParamType::SIZE, timeKoeff);
            }
        }

        void ParticleAnimation::getTransform(float animKoeff, math::m4x4 &out) const {
            unsigned firstFrameIndex = unsigned(animKoeff * float(_frameCount));
            
            const math::p3d &pos = _animationFrames[firstFrameIndex].position;
            float scl = _animationFrames[firstFrameIndex].size;

            out.setScaling(scl, scl, scl);
            out._41 = pos.x;
            out._42 = pos.y;
            out._43 = pos.z;
        }

        unsigned ParticleAnimation::getFrameCount() const {
            return _frameCount;
        }

        float ParticleAnimation::getLifeTimeMs() const {
            return _lifeTimeMs;
        }

        float ParticleAnimation::getBornTimeMs() const {
            return _bornTimeMs;
        }
        
        float ParticleAnimation::_getModifiedParticleParam(const std::unordered_map <ParticleParamType, Modifier *> &modifiers, ParticleParamType paramType, float koeff) const {
            auto  index = modifiers.find(paramType);
            float value = _dynamicParams[unsigned(paramType)];

            if(index != modifiers.end()) {
                value = index->second->modify(koeff, value);
            }

            return value; 
        }

        //---------------------------------------------------------------------

        ParticleEmitter::ParticleEmitter() {
            _nrmLifePeriodMs = 1.0f;
            _lifeTimeMs = 100.0f;
            _frameTimeMs = 33.34f;

            _dynamicParams[unsigned(EmitterParamType::PARTICLES_PER_SEC)] = 10.f;
            _dynamicParams[unsigned(EmitterParamType::VELOCITY_MIN)] = 0.5f;
            _dynamicParams[unsigned(EmitterParamType::VELOCITY_MAX)] = 1.0f;
            _dynamicParams[unsigned(EmitterParamType::LIFETIME_MIN)] = 2200.0f;
            _dynamicParams[unsigned(EmitterParamType::LIFETIME_MAX)] = 2200.0f;
            _dynamicParams[unsigned(EmitterParamType::CONE_ANGLE_MIN)] = 45.0f;
            _dynamicParams[unsigned(EmitterParamType::CONE_ANGLE_MAX)] = 45.0f;
            _dynamicParams[unsigned(EmitterParamType::PARTICLE_SIZE_MIN)] = 1.0f;
            _dynamicParams[unsigned(EmitterParamType::PARTICLE_SIZE_MAX)] = 1.0f;
            _dynamicParams[unsigned(EmitterParamType::ANGLE_MIN)] = 0.0f;
            _dynamicParams[unsigned(EmitterParamType::ANGLE_MAX)] = 0.0f;
            _dynamicParams[unsigned(EmitterParamType::ANGLE_VELOCITY_MIN)] = 0.0f;
            _dynamicParams[unsigned(EmitterParamType::ANGLE_VELOCITY_MAX)] = 0.0f;
            _dynamicParams[unsigned(EmitterParamType::TORSION_MIN)] = 0.0f;
            _dynamicParams[unsigned(EmitterParamType::TORSION_MAX)] = 0.0f;

            _cycled = true;
            _torsionAxis = math::p3d(0, 1, 0);

            _curTimeMs = 0.0f;
            _curNrmTimeMs = 0.0f;
            _curParticleIndex = 0;
        }

        ParticleEmitter::~ParticleEmitter() {
            for(auto index = _particles.begin(); index != _particles.end(); ++index) {
                delete *index;
            }

            for(auto index = _emitterModifiers.begin(); index != _emitterModifiers.end(); ++index) {
                delete index->second;
            }
            for(auto index = _particleModifiers.begin(); index != _particleModifiers.end(); ++index) {
                delete index->second;
            }
        }

        float ParticleEmitter::_getModifiedEmitterRandomParam(EmitterParamType paramTypeMin, EmitterParamType paramTypeMax, float koeff) const {
            float  tmin = _getModifiedEmitterParam(paramTypeMin, koeff);
            float  tmax = _getModifiedEmitterParam(paramTypeMax, koeff);
            float  dist = max(tmax - tmin, 0.0f);
            float  rndf = float(rand() % 1000) / 1000.0f;
            return tmin + dist * rndf;
        }

        float ParticleEmitter::_getModifiedEmitterParam(EmitterParamType paramType, float koeff) const {
            auto  index = _emitterModifiers.find(paramType);
            float value = _dynamicParams[unsigned(paramType)];

            if(index != _emitterModifiers.end()) {
                value = index->second->modify(koeff, value);
            }

            return value; 
        }

        float ParticleEmitter::_getMaximumEmitterParam(EmitterParamType paramType) const {
            auto  index = _emitterModifiers.find(paramType);
            float value = _dynamicParams[unsigned(paramType)];

            if(index != _emitterModifiers.end()) {
                value *= index->second->getMaximum();
            }

            return value; 
        }

        float ParticleEmitter::_getMinimumEmitterParam(EmitterParamType paramType) const {
            auto  index = _emitterModifiers.find(paramType);
            float value = _dynamicParams[unsigned(paramType)];

            if(index != _emitterModifiers.end()) {
                value *= index->second->getMinimum();
            }

            return value; 
        }

        void ParticleEmitter::_getConeRandomVectorAroundY(float maxAngle, math::p3d &out) const {
            float Y = cosf(maxAngle / 180.0f * M_PI);
            float root = sqrtf(1.0f - Y * Y);
            float randKoeff = float(rand() % 2000) / 1000.0f * M_PI;

            out.x = root * cosf(randKoeff);
            out.z = root * sinf(randKoeff);
            out.y = Y;
        }

        void ParticleEmitter::build() {
            for(auto index = _particles.begin(); index != _particles.end(); ++index) {
                delete *index;
            }
            
            float  maxParticlelifeMs = _getMaximumEmitterParam(EmitterParamType::LIFETIME_MAX);
            float  minParticleBornPeriodMs = 1000.0f / _getMaximumEmitterParam(EmitterParamType::PARTICLES_PER_SEC);
            
            _nrmLifePeriodMs = _lifeTimeMs > maxParticlelifeMs ? _lifeTimeMs : maxParticlelifeMs;
            _nrmLifePeriodMs += _lifeTimeMs - fmod(_nrmLifePeriodMs, _lifeTimeMs);
            _nrmLifePeriodMs += minParticleBornPeriodMs - fmod(_nrmLifePeriodMs, minParticleBornPeriodMs);

            float  accumulatedMs = 0.0f;
            float  accumulatedParticles = 0.0f;
            float  evalTime = _cycled ? _nrmLifePeriodMs : _lifeTimeMs;

            while(accumulatedMs < evalTime) {
                float nrmTime = fmod(accumulatedMs, _lifeTimeMs);
                float emitterKoeff = nrmTime / _lifeTimeMs;
                
                accumulatedParticles += _getModifiedEmitterParam(EmitterParamType::PARTICLES_PER_SEC, emitterKoeff);
                accumulatedMs += 1.0f;
                
                if(unsigned(accumulatedParticles * 0.001f) > _particles.size()) {
                    float bornLifeTime  = _getModifiedEmitterRandomParam(EmitterParamType::LIFETIME_MIN, EmitterParamType::LIFETIME_MAX, emitterKoeff);
                    float bornConeAngle = _getModifiedEmitterRandomParam(EmitterParamType::CONE_ANGLE_MIN, EmitterParamType::CONE_ANGLE_MAX, emitterKoeff);

                    math::p3d coneDir;
                    _getConeRandomVectorAroundY(bornConeAngle, coneDir);

                    ParticleBornParams bornParams;
                    bornParams.velocity = _getModifiedEmitterRandomParam(EmitterParamType::VELOCITY_MIN, EmitterParamType::VELOCITY_MAX, emitterKoeff);
                    bornParams.avelocity = _getModifiedEmitterRandomParam(EmitterParamType::ANGLE_VELOCITY_MIN, EmitterParamType::ANGLE_VELOCITY_MAX, emitterKoeff);
                    bornParams.torsion = _getModifiedEmitterRandomParam(EmitterParamType::TORSION_MIN, EmitterParamType::TORSION_MAX, emitterKoeff);
                    bornParams.angle = _getModifiedEmitterRandomParam(EmitterParamType::ANGLE_MIN, EmitterParamType::ANGLE_MAX, emitterKoeff);
                    bornParams.size = _getModifiedEmitterRandomParam(EmitterParamType::PARTICLE_SIZE_MIN, EmitterParamType::PARTICLE_SIZE_MAX, emitterKoeff);

                    _particles.resize(_particles.size() + 1);
                    _particles.back() = new ParticleAnimation(accumulatedMs, bornLifeTime, _frameTimeMs);
                    _particles.back()->init(math::p3d(), coneDir, bornParams);
                    _particles.back()->initFrames(_particleModifiers, _torsionAxis);
                }
            }

            _curNrmTimeMs = 1.0f;
            _curParticleIndex = 0;
        }

        void ParticleEmitter::setTimeStamp(float timeMs) {
            if(_cycled) {
                _curTimeMs = timeMs;
                _curNrmTimeMs = fmod(timeMs, _nrmLifePeriodMs);
            }
            else {
                _curTimeMs = min(timeMs, _lifeTimeMs);
                _curNrmTimeMs = timeMs;
            }

            _curParticleIndex = 0;
        }

        bool ParticleEmitter::getNextParticleData(math::m4x4 &trfm) const {
            while(_curParticleIndex < unsigned(_particles.size())) {
                const ParticleAnimation *cur = _particles[_curParticleIndex]; 
                float localTimeMs = _curNrmTimeMs - cur->getBornTimeMs();

                localTimeMs += localTimeMs < 0.0f ? _nrmLifePeriodMs : 0.0f;
                _curParticleIndex++;

                if(cur->getBornTimeMs() < _curTimeMs && localTimeMs < cur->getLifeTimeMs()) {
                    cur->getTransform(localTimeMs / cur->getLifeTimeMs(), trfm);
                    return true;
                }
            }

            _curParticleIndex = 0;
            return false;
        }

        ModifierInterface *ParticleEmitter::createEmitterModifier(EmitterParamType type) {
            Modifier *mdf = nullptr;
            
            if(_emitterModifiers.find(type) == _emitterModifiers.end()) {
                 mdf = new Modifier();
                _emitterModifiers.emplace(type, mdf);
            }
                        
            return mdf;
        }

        ModifierInterface *ParticleEmitter::createParticleModifier(ParticleParamType type) {
            Modifier *mdf = nullptr;

            if(_particleModifiers.find(type) == _particleModifiers.end()) {
                mdf = new Modifier();
                _particleModifiers.emplace(type, mdf);
            }

            return mdf;
        }

        void ParticleEmitter::removeEmitterModifier(EmitterParamType type) {
            auto index = _emitterModifiers.find(type);

            if(index != _emitterModifiers.end()) {
                delete index->second;
                _emitterModifiers.erase(index);
            }
        }

        void ParticleEmitter::removeParticleModifier(ParticleParamType type) {
            auto index = _particleModifiers.find(type);

            if(index != _particleModifiers.end()) {
                delete index->second;
                _particleModifiers.erase(index);
            }
        }

        void ParticleEmitter::setTorsionAxis(const math::p3d &axis) {
            float tlen = axis.length();

            if(tlen > M_EPSILON) {
                _torsionAxis = axis / tlen;
            }
            else {
                _torsionAxis = math::p3d(0, 1, 0);
            }
        }

        void ParticleEmitter::setParam(EmitterParamType param, float value) {
            _dynamicParams[unsigned(param)] = value;
        }

        void ParticleEmitter::setFps(float framesPerSecond) {
            _frameTimeMs = 1000.0f / framesPerSecond;
        }

        void ParticleEmitter::setLifeTime(float lifeTimeMs) {
            _lifeTimeMs = lifeTimeMs;
        }

        void ParticleEmitter::setCycled(bool cycled) {
            _cycled = cycled;
        }

        float ParticleEmitter::getParam(EmitterParamType param) const {
            return _dynamicParams[unsigned(param)];
        }

        float ParticleEmitter::getFps() const {
            return 1000.0f / _frameTimeMs;
        }

        float ParticleEmitter::getLifeTime() const {
            return _lifeTimeMs;
        }

        bool ParticleEmitter::isCycled() const {
            return _cycled;
        }
    }
}