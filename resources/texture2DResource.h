
namespace fg {
    namespace resources {
        class Texture2DResource : public Texture2DResourceInterface, public Resource {
        public:
            Texture2DResource(const char *path, bool unloadable) : Resource(path, unloadable), _mipsCount(0), _imgData(nullptr), _self(nullptr) {}
            ~Texture2DResource() override;

            void loaded(const diag::LogInterface &log) override;
            bool constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void unloaded() override;

            unsigned getWidth() const override;
            unsigned getHeight() const override;

            virtual const platform::Texture2DInterface *getPlatformObject() const override;

        private:
            unsigned _szx;
            unsigned _szy;
            unsigned _mipsCount;
            
            unsigned char  **_imgData;
            platform::Texture2DInterface  *_self;
        };
    }
}


