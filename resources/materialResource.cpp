
namespace fg {
    namespace resources {
        MaterialResource::~MaterialResource() {
            _meshes.foreach([](const fg::string &, MaterialMeshParams *ptr){
                delete ptr;
            });
        }

        void MaterialResource::loaded(const diag::LogInterface &log) {
            luaScript cfgSource;

            if(cfgSource.execLuaChunk((char *)_binaryData, _binarySize)){
                luaObj cfg;

                cfgSource.getGlobalVar("roots", cfg);
                _addMaterial(_roots, cfg);
                cfgSource.getGlobalVar("material", cfg);
                _addMaterial(_meshes, cfg);
            }
            else{
                log.msgError("MaterialResource::loaded %s - %s", _loadPath.data(), cfgSource.getLastError());
                _loadingState = ResourceLoadingState::INVALID;
            }
        }

        bool MaterialResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            return false; // false = unload source binary
        }

        void MaterialResource::unloaded() {
            _meshes.foreach([](const fg::string &, MaterialMeshParams *ptr) {
                delete ptr;
            });

            _meshes.clear();
        }
        
        const MaterialMeshParams *MaterialResource::getMeshParams(const fg::string &meshName) const {
            return _meshes.get(meshName);
        }

        const MaterialMeshParams *MaterialResource::getMeshSubTreeParams(const fg::string &rootName) const {
            if(rootName == "obs_rot_bridge") {
                printf("");
            }

            return _roots.get(rootName);
        }

        void MaterialResource::_addMaterial(StaticHash <FG_MATERIAL_ENTRY_MAX, MaterialMeshParams *> &target, const luaObj &cfg) {
            if(cfg.type() == LUATYPE_TABLE) {
                cfg.foreach([&target](const char *meshName, luaObj &params) {
                    MaterialMeshParams *curParams = new MaterialMeshParams();

                    curParams->textureBindCount = 0;
                    curParams->shaderPath = (const char *)params.get("shader");

                    const luaObj &textureBinds = params.get("textureBinds");
                    if(textureBinds.type() == LUATYPE_TABLE) {
                        for(unsigned i = 0; i < textureBinds.count(); i++) {
                            curParams->textureBinds[curParams->textureBindCount] = (const char *)textureBinds.get(i + 1);
                            curParams->textureBindCount++;
                        }
                    }

                    const luaObj &metalness = params.get("metalness");
                    
                    if (metalness.type() == LUATYPE_TABLE) {
                        curParams->metalness = math::p3d(metalness.get(1), metalness.get(2), metalness.get(3));
                    }
                    else {
                        curParams->metalness = math::p3d(metalness, metalness, metalness);
                    }

                    curParams->glossiness = params.get("glossiness");
                    curParams->isTransparent = params.get("isTransparent");

                    target.add(meshName, curParams);
                    return true; // continue cycle
                });
            }
        }
    }
}

