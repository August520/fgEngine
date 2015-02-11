
namespace fg {
    namespace resources {
        class SoundResource : public SoundResourceInterface, public Resource {
        public:
            SoundResource(const char *path, bool unloadable) : Resource(path, unloadable) {}
            ~SoundResource() override {}

            void  loaded(const diag::LogInterface &log) override;
            bool  constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void  unloaded() override;
            
            unsigned getSize() const override;
            const unsigned char *getChunkPtr(unsigned offset) const override;
        };
    }
}
