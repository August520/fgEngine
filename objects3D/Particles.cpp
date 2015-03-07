
namespace fg {
    namespace object3d {
        Particles3D::EmitterData::EmitterData(particles::EmitterInterface *emitter, const Particles3D &owner) :
            _emitter(emitter), 
            _shader(nullptr),
            _owner(owner) 
        {
            for(unsigned i = 0; i < resources::FG_MATERIAL_TEXTURE_MAX; i++) {
                _textureBinds[i] = nullptr;
            }
        }

        Particles3D::EmitterData::~EmitterData() {
        
        }

        bool Particles3D::EmitterData::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            if(_shader == nullptr) {
                if((_shader = resMan.getResource(_emitter->getShader())) == nullptr) {
                    return false;
                }
            }

            for(unsigned i = 0; i < _emitter->getTextureBindCount(); i++) {
                if(_textureBinds[i] == nullptr) {
                    if((_textureBinds[i] = resMan.getResource(_emitter->getTextureBind(i))) == nullptr) {
                        return false;
                    }
                }

                if(_textureBinds[i]->valid() == false) {
                    return false;
                }
            }

            return _shader->valid();
        }

        unsigned Particles3D::EmitterData::getTextureBindCount() const {
            return _emitter->getTextureBindCount();
        }

        const math::m4x4 &Particles3D::EmitterData::getFullTransform() const {
            return _owner.getFullTransform();
        }

        const particles::EmitterInterface *Particles3D::EmitterData::getEmitter() const {
            return _emitter;
        }

        const resources::ShaderResourceInterface *Particles3D::EmitterData::getShader() const {
            return _shader;
        }

        const resources::Texture2DResourceInterface *Particles3D::EmitterData::getTextureBind(unsigned bindIndex) const {
            return _textureBinds[bindIndex];
        }
        
        particles::EmitterInterface *Particles3D::EmitterData::getEmitter() {
            return _emitter;
        }

        const math::m4x4 &Particles3D::EmitterData::getTransformHistory(float timeBeforeMs) const {
            unsigned index = (unsigned(timeBeforeMs / 10.0f) + _owner._transformHistoryIndex + 1) % _owner._transformHistorySize;
            return _owner._transformHistoryData[index];
        }

        //---
                    
        Particles3D::Particles3D() : _particles(nullptr), _transformHistoryData(nullptr) {
            _type = RenderObjectType::PARTICLES;
            _timeElapsed = 0.0f;
            _transformHistorySize = 0;
            _transformHistoryIndex = 0;
        }

        Particles3D::~Particles3D() {
            for(auto index = _emitters.begin(); index != _emitters.end(); ++index) {
                delete *index;
            }

            delete [] _transformHistoryData;
        }

        void Particles3D::setResource(const fg::string &particlesResourcePath) {
            _particlesResourcePath = particlesResourcePath;
        }

        particles::EmitterInterface *Particles3D::getEmitter(const fg::string &name) const {
            if(_particles) {
                return _particles->getEmitter(name);
            }

            return nullptr;
        }

        void Particles3D::updateCoordinates(float frameTimeMs) {
            RenderObject::updateCoordinates(frameTimeMs);
            _timeElapsed += frameTimeMs;

            if(_transformHistoryData) {
                unsigned nextHistoryIndex = _transformHistorySize - unsigned(_timeElapsed / 10.0f) % _transformHistorySize - 1;
            
                while(_transformHistoryIndex != nextHistoryIndex) {
                    _transformHistoryData[_transformHistoryIndex] = _fullTransform;
                    _transformHistoryIndex > 0 ? _transformHistoryIndex-- : _transformHistoryIndex = _transformHistorySize - 1;
                }
            }
            
            for(auto index = _emitters.begin(); index != _emitters.end(); ++index) {
                (*index)->getEmitter()->setTimeStamp(_timeElapsed);
            }
        }

        bool Particles3D::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            if(_particles == nullptr) {
                if((_particles = resMan.getResource(_particlesResourcePath)) == nullptr) {
                    return false;
                }
            }

            if(_particles->valid()) {
                if(_emitters.size() == 0) {
                    float maxParticleLifeTime = 1.0f;
                    
                    std::vector <particles::EmitterInterface *> rawEmitters;
                    _particles->getEmitters(rawEmitters);

                    for(auto index = rawEmitters.begin(); index != rawEmitters.end(); ++index) {
                        EmitterData *tdata = new EmitterData (*index, *this); 
                        _emitters.push_back(tdata);
                    
                        if((*index)->getMaxParticleLifeTime() > maxParticleLifeTime) {
                            maxParticleLifeTime = (*index)->getMaxParticleLifeTime();
                        }
                    }

                    _transformHistorySize = unsigned(maxParticleLifeTime / 10.0f) + 1;
                    _transformHistoryData = new math::m4x4 [_transformHistorySize];
                    _transformHistoryIndex = _transformHistorySize - 1;
                }

                return true;
            }

            for(auto index = _emitters.begin(); index != _emitters.end(); ++index) {
                delete *index;
            }

            delete[] _transformHistoryData;

            _transformHistorySize = 0;
            _transformHistoryData = nullptr;

            _emitters.clear();
            return false;
        }

        unsigned Particles3D::getComponentCount() const {
            return unsigned(_emitters.size());
        }

        RenderObject::ComponentInterface *Particles3D::getComponentInterface(unsigned index) {
            return _emitters[index];
        }
    }
}


//