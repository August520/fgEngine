
namespace fg {
    namespace object3d {
        class Particles3D : public Particles3DInterface, public RenderObject {
        public:
            class EmitterData : public Particles3DInterface::EmitterComponentInterface {
                friend class Particles3D;

            public:
                EmitterData(particles::EmitterInterface *emitter, const Particles3D &owner);
                ~EmitterData() override;

                bool isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;
                unsigned getTextureBindCount() const override;

                const math::m4x4 &getFullTransform() const override;
                const particles::EmitterInterface *getEmitter() const override;
                const resources::ShaderResourceInterface *getShader() const override;
                const resources::Texture2DResourceInterface *getTextureBind(unsigned bindIndex) const override;

                const math::m4x4 &getTransformHistory(float timeBeforeMs) const override;

            protected:
                const Particles3D            &_owner;
                particles::EmitterInterface  *_emitter;

                const resources::Texture2DResourceInterface  *_textureBinds[resources::FG_MATERIAL_TEXTURE_MAX];
                const resources::ShaderResourceInterface     *_shader;
                        
                particles::EmitterInterface *getEmitter();
            };
            
            Particles3D();
            ~Particles3D() override;
            
            particles::EmitterInterface *getEmitter(const fg::string &name) const override;

            void  setResource(const fg::string &particlesResourcePath) override;
            void  updateCoordinates(float frameTimeMs) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

            unsigned  getComponentCount() const override;
            ComponentInterface *getComponentInterface(unsigned index) override;

        protected:
            float        _timeElapsed;
            fg::string   _particlesResourcePath;
            std::vector  <EmitterData *> _emitters;
            math::m4x4   *_transformHistoryData;
            unsigned     _transformHistorySize;
            unsigned     _transformHistoryIndex;

            const resources::ParticleResourceInterface *_particles;
        };
    }
}
