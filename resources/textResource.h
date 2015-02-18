
// TODO: unicode?

namespace fg {
    namespace resources {
        class TextResource : public TextResourceInterface, public Resource {
        public:
            TextResource(const char *path, bool unloadable) : Resource(path, unloadable), _text(0), _size(0) {}
            ~TextResource() override;

            void loaded(const diag::LogInterface &log) override;
            bool constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void unloaded() override;
            void save(void **outBinary, unsigned *outSize) const override;

            void setText(const char *txt, unsigned size);
            bool commit() override;
            
            const char *getText() const override;
            unsigned    getSize() const override;
                        
        private:
            char *_text;
            unsigned _size;
        };
    }
}

