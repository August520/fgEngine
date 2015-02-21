
namespace fg {
    namespace particles {
        ParticleAnimation::ParticleAnimation() : _animationFrames(nullptr) {

        }

        ParticleAnimation::~ParticleAnimation() {
            delete[] _animationFrames;
        }

        void ParticleAnimation::init(float bornTimeMs, float lifeTimeMs, float frameTimeMs, const math::p3d &startPos) {
            _startPos = startPos;
            _frameCount = unsigned(lifeTimeMs / frameTimeMs) + 1;
            _animationFrames = new Frame[_frameCount];
            _bornTimeMs = bornTimeMs;
            _lifeTimeMs = lifeTimeMs;
        }

        void ParticleAnimation::initFrame(unsigned frameIndex, const Modifier *modifiers) {
            float localTimeMs = float(frameIndex) / float(_frameCount) * _lifeTimeMs;
            _animationFrames[frameIndex].localPosition = _startPos + math::p3d(0, 0.001f, 0) * localTimeMs;
        }

        void ParticleAnimation::getTransform(float animKoeff, math::m4x4 &out) const {
            unsigned firstFrameIndex = unsigned(animKoeff * float(_frameCount));

            _animationFrames[firstFrameIndex].localRotation.toMatrix(out);

            const math::p3d &scl = _animationFrames[firstFrameIndex].localScale;
            const math::p3d &pos = _animationFrames[firstFrameIndex].localPosition;

            (*(math::p3d *)&out._11).toLength(scl.x);
            (*(math::p3d *)&out._21).toLength(scl.y);
            (*(math::p3d *)&out._31).toLength(scl.z);

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

        //---------------------------------------------------------------------

        ParticleEmitter::ParticleEmitter() {
            _nrmLifePeriodMs = 1.0f;
            _lifeTimeMs = 500.0f;
            _frameTimeMs = 33.33333f;

            _dynamicParams[unsigned(EmitterModifierType::BORN_PERIOD)] = 200.0f;
            _dynamicParams[unsigned(EmitterModifierType::VELOCITY_MIN)] = 0.01f;
            _dynamicParams[unsigned(EmitterModifierType::VELOCITY_MAX)] = 0.01f;
            _dynamicParams[unsigned(EmitterModifierType::LIFETIME_MIN)] = 1000.0f;
            _dynamicParams[unsigned(EmitterModifierType::LIFETIME_MAX)] = 1000.0f;
            _dynamicParams[unsigned(EmitterModifierType::CONE_ANGLE_MIN)] = 0.0f;
            _dynamicParams[unsigned(EmitterModifierType::CONE_ANGLE_MAX)] = 45.0f;
            _dynamicParams[unsigned(EmitterModifierType::PARTICLE_SIZE_MIN)] = 1.0f;
            _dynamicParams[unsigned(EmitterModifierType::PARTICLE_SIZE_MAX)] = 1.0f;
            _dynamicParams[unsigned(EmitterModifierType::ANGLE_MIN)] = 0.0f;
            _dynamicParams[unsigned(EmitterModifierType::ANGLE_MAX)] = 0.0f;
            _dynamicParams[unsigned(EmitterModifierType::ANGLE_VELOCITY_MIN)] = 0.0f;
            _dynamicParams[unsigned(EmitterModifierType::ANGLE_VELOCITY_MAX)] = 0.0f;
            _dynamicParams[unsigned(EmitterModifierType::TORSION_MIN)] = 0.0f;
            _dynamicParams[unsigned(EmitterModifierType::TORSION_MAX)] = 0.0f;

            _particles = nullptr;
            _maxParticles = 0;
            _cycled = true;

            _curTimeMs = 0.0f;
            _curNrmTimeMs = 0.0f;
            _curParticleIndex = 0;
            _curYoungestParticleIndex = 0;
        }

        ParticleEmitter::~ParticleEmitter() {

        }

        void ParticleEmitter::build() {
            delete _particles;
            float  maxParticlelifeMs = _dynamicParams[unsigned(EmitterModifierType::LIFETIME_MAX)];
            float  particleBornPeriod = _dynamicParams[unsigned(EmitterModifierType::BORN_PERIOD)];

            _nrmLifePeriodMs = _lifeTimeMs > maxParticlelifeMs ? _lifeTimeMs : maxParticlelifeMs;
            _nrmLifePeriodMs += particleBornPeriod - fmod(_nrmLifePeriodMs, particleBornPeriod);

            _maxParticles = unsigned(_nrmLifePeriodMs / particleBornPeriod + 0.5f);
            _particles = new ParticleAnimation[_maxParticles];

            for(int i = int(_maxParticles) - 1; i >= 0; i--) {
                float bornTimeMs = float(int(_maxParticles) - i - 1) * particleBornPeriod;

                _particles[i].init(bornTimeMs, maxParticlelifeMs, _frameTimeMs, math::p3d());

                for(unsigned c = 0; c < _particles[i].getFrameCount(); c++) {
                    _particles[i].initFrame(c, nullptr);
                }
            }

            _curNrmTimeMs = 0.0f;
            _curParticleIndex = 0;
            _curYoungestParticleIndex = int(_maxParticles) - 1;
        }

        void ParticleEmitter::setTimeStamp(float timeMs) {
            float  particleBornPeriod = _dynamicParams[unsigned(EmitterModifierType::BORN_PERIOD)];
            
            _curTimeMs = timeMs;
            _curNrmTimeMs = _cycled ? fmod(timeMs, _nrmLifePeriodMs) : timeMs;
            _curYoungestParticleIndex = _maxParticles - unsigned(_curNrmTimeMs / particleBornPeriod) - 1;
            _curParticleIndex = 0;
        }

        bool ParticleEmitter::getNextParticleData(math::m4x4 &trfm) {
            while(_curParticleIndex < _maxParticles) {
                const ParticleAnimation &cur = _particles[(_curYoungestParticleIndex + _curParticleIndex) % _maxParticles];
                float localTimeMs = _curNrmTimeMs - cur.getBornTimeMs();

                localTimeMs += localTimeMs < 0.0f ? _nrmLifePeriodMs : 0.0f;
                _curParticleIndex++;

                if(cur.getBornTimeMs() < _curTimeMs && localTimeMs < cur.getLifeTimeMs()) {
                    cur.getTransform(localTimeMs / cur.getLifeTimeMs(), trfm);
                    return true;
                }
            }

            _curParticleIndex = 0;
            return false;
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

        float ParticleEmitter::getFps() {
            return 1000.0f / _frameTimeMs;
        }

        float ParticleEmitter::getLifeTime() {
            return _lifeTimeMs;
        }

        bool ParticleEmitter::isCycled() {
            return _cycled;
        }
    }
}