
// TODO: unicode?

namespace fg {
    namespace resources {
        class ParticleResource : public ParticleResourceInterface, public Resource {
        public:
            ParticleResource(const char *path, bool unloadable) : Resource(path, unloadable) {}
            ~ParticleResource() override;

            void loaded(const diag::LogInterface &log) override;
            bool constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void unloaded() override;
            
            void getEmitters(std::vector <particles::EmitterInterface *> &out) const override;
            particles::EmitterInterface *getEmitter(const fg::string &name) const override;

        private:
            StaticHash  <FG_EMITTERS_MAX, particles::Emitter *> _emitters;
        };
    }
}

