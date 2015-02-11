
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
                cfgSource.getGlobalVar("material", cfg);

                if(cfg.type() == LUATYPE_TABLE){
                    cfg.foreach([this](const char *meshName, luaObj &params){
                        MaterialMeshParams *curParams = new MaterialMeshParams();

                        curParams->textureBindCount = 0;
                        curParams->shaderPath = (const char *)params.get("shader");
                        
                        const luaObj &textureBinds = params.get("textureBinds");
                        if(textureBinds.type() == LUATYPE_TABLE){
                            textureBinds.foreach([curParams](const luaObj &varName, const luaObj &textureName){
                                curParams->textureBinds[curParams->textureBindCount] = (const char *)textureName;
                                curParams->textureBindCount++;
                                return true;
                            });
                        }

                        //curParams->userData = params.get("userData");
                        
                        _meshes.add(meshName, curParams);
                        return true; // continue cycle
                    });
                }
            }
            else{
                log.msgError("MaterialResource::loaded %s - %s", _loadPath, cfgSource.getLastError());
                _state = ResourceState::INVALID;
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
    }
}

