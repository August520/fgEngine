
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

            const char *getText() const override;
            int   getSize() const override;

        private:
            char *_text;
            int  _size;
        };
    }
}

