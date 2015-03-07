
namespace fg {
    namespace object3d {
        Particles3D::EmitterData::EmitterData(particles::EmitterInterface *emitter, const math::m4x4 &ownerTransform) : 
            _emitter(emitter), 
            _shader(nullptr),
            _ownerTransform(ownerTransform) 
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
            return _ownerTransform;
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

        //---
                    
        Particles3D::Particles3D() : _particles(nullptr) {
            _type = RenderObjectType::PARTICLES;
            _timeElapsed = 0.0f;
        }

        Particles3D::~Particles3D() {
            for(auto index = _emitters.begin(); index != _emitters.end(); ++index) {
                delete *index;
            }
        }

        void Particles3D::setResource(const fg::string &particlesResourcePath) {
            _particlesResourcePath = particlesResourcePath;
        }

        particles::EmitterInterface *Particles3D::addEmitter(const fg::string &name) {
            if(_particles) {
                
            }

            return nullptr;
        }

        particles::EmitterInterface *Particles3D::getEmitter(const fg::string &name) const {
            if(_particles) {
                return _particles->getEmitter(name);
            }

            return nullptr;
        }

        void Particles3D::removeEmitter(const fg::string &name) {
        
        }

        void Particles3D::buildEmitters() {
        
        }

        void Particles3D::updateCoordinates(float frameTimeMs) {
            RenderObject::updateCoordinates(frameTimeMs);
            _timeElapsed += frameTimeMs;
            
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
                    std::vector <particles::EmitterInterface *> rawEmitters;

                    _particles->getEmitters(rawEmitters);

                    for(auto index = rawEmitters.begin(); index != rawEmitters.end(); ++index) {
                        EmitterData *tdata = new EmitterData (*index, _fullTransform); 
                        _emitters.push_back(tdata);
                    }
                }

                return true;
            }

            for(auto index = _emitters.begin(); index != _emitters.end(); ++index) {
                delete *index;
            }

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