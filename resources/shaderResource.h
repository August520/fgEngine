
namespace fg {
    namespace resources {
        class ShaderResource : public Resource, public ShaderResourceInterface {
        public:
            ShaderResource(const char *path, bool unloadable) : Resource(path, unloadable), _self(nullptr) {}
            ~ShaderResource() override;

            void loaded(const diag::LogInterface &log) override;
            bool constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void unloaded() override;

            const platform::ShaderInterface *getPlatformObject() const override;

        private:
            platform::ShaderInterface *_self;
        };
    }
}

