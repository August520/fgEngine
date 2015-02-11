
namespace fg {
    namespace resources {
        class MaterialResource : public MaterialResourceInterface, public Resource {
        public:
            MaterialResource(const char *path, bool unloadable) : Resource(path, unloadable) {}
            ~MaterialResource() override;

            void  loaded(const diag::LogInterface &log) override;
            bool  constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void  unloaded() override;

            const MaterialMeshParams *getMeshParams(const fg::string &meshName) const override;

        protected:
            StaticHash <FG_MESH_MAX, MaterialMeshParams *> _meshes;
        };
    }
}

