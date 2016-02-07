
namespace fg {
    namespace resources {
        class TextureCubeResource : public TextureCubeResourceInterface, public Resource {
        public:
            TextureCubeResource(const char *path, bool unloadable) : Resource(path, unloadable), _mipsCount(0), _self(nullptr) {}
            ~TextureCubeResource() override;

            void loaded(const diag::LogInterface &log) override;
            bool constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void unloaded() override;

            virtual const platform::TextureCubeInterface *getPlatformObject() const override;

        private:
            unsigned _sz = 1;
            unsigned _mipsCount = 1;

            unsigned char **_imgData[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
            platform::TextureFormat _format = platform::TextureFormat::UNKNOWN;
            platform::TextureCubeInterface *_self = nullptr;
        };
    }
}


