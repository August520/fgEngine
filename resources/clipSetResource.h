
namespace fg {
    namespace resources {
        class ClipSetResource : public ClipSetResourceInterface, public Resource {
        public:
            ClipSetResource(const char *path, bool unloadable) : Resource(path, unloadable) {}
            ~ClipSetResource() override;

            void  loaded(const diag::LogInterface &log) override;
            bool  constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void  unloaded() override;

            const ClipData  *getClip(const fg::string &name) const override;

        protected:
            std::unordered_map  <std::string, ClipData *> _clips;
        };
    }
}


//