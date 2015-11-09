
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
            const MaterialMeshParams *getMeshSubTreeParams(const fg::string &rootName) const override;

        protected:
            StaticHash <FG_MATERIAL_ENTRY_MAX, MaterialMeshParams *> _roots;
            StaticHash <FG_MATERIAL_ENTRY_MAX, MaterialMeshParams *> _meshes;

            void _addMaterial(StaticHash <FG_MATERIAL_ENTRY_MAX, MaterialMeshParams *> &target, const luaObj &cfg);
        };
    }
}

