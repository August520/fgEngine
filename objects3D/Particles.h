
namespace fg {
    namespace object3d {
        class Particles3D : public Particles3DInterface, public RenderObject {
        public:
            class EmitterData : public Particles3DInterface::EmitterComponentInterface {
                friend class Particles3D;

            public:
                EmitterData(particles::EmitterInterface *emitter, const math::m4x4 &ownerTransform);
                ~EmitterData() override;

                bool isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;
                unsigned getTextureBindCount() const override;

                const math::m4x4 &getFullTransform() const override;
                const particles::EmitterInterface *getEmitter() const override;
                const resources::ShaderResourceInterface *getShader() const override;
                const resources::Texture2DResourceInterface *getTextureBind(unsigned bindIndex) const override;

            protected:
                const math::m4x4             &_ownerTransform;
                particles::EmitterInterface  *_emitter;

                const resources::Texture2DResourceInterface  *_textureBinds[resources::FG_MATERIAL_TEXTURE_MAX];
                const resources::ShaderResourceInterface     *_shader;
                        
                particles::EmitterInterface *getEmitter();
            };
            
            Particles3D();
            ~Particles3D() override;
                        
            void  setResource(const fg::string &particlesResourcePath) override;

            particles::EmitterInterface *addEmitter(const fg::string &name) override;
            particles::EmitterInterface *getEmitter(const fg::string &name) const override;

            void  removeEmitter(const fg::string &name) override;
            void  buildEmitters() override;

            void  updateCoordinates(float frameTimeMs) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

            unsigned  getComponentCount() const override;
            ComponentInterface *getComponentInterface(unsigned index) override;

        protected:
            float        _timeElapsed;
            fg::string   _particlesResourcePath;
            std::vector  <EmitterData *> _emitters;

            const resources::ParticleResourceInterface *_particles;
        };
    }
}
