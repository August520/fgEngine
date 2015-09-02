
//TODO: default values

namespace fg {
    namespace resources {
        ClipSetResource::~ClipSetResource() {
            for(auto index = _clips.begin(); index != _clips.end(); ++index){
                delete index->second->boundingCoords;
                delete index->second->frames;
                delete index->second;
            }
        }

        void ClipSetResource::loaded(const diag::LogInterface &log) {
            luaScript src;
            if(src.execLuaChunk((char *)_binaryData, _binarySize)) {
                luaObj cfg = src.getGlobalVar("clipset");

                cfg.foreach([this](const luaObj &key, luaObj &value){
                    if(value.type() == LUATYPE_TABLE){
                        ClipData *tclip = new ClipData();
                        tclip->width = value.get("width");
                        tclip->height = value.get("height");
                        tclip->centerX = value.get("centerX");
                        tclip->centerY = value.get("centerY");
                        tclip->frameRate = value.get("frameRate");

                        tclip->frameCount = value.get("frameCount");
                        tclip->frames = new ClipData::Frame [tclip->frameCount];

                        for(unsigned int i = 0; i < tclip->frameCount; i++){
                            const luaObj &curFrameDat = value.get(i);
                            tclip->frames[i].tu = curFrameDat.get("tu");
                            tclip->frames[i].tv = curFrameDat.get("tv");
                        }

                        const luaObj &boundsSrc = value.get("bounds");
                        if(boundsSrc.type() == LUATYPE_TABLE){
                            tclip->boundingCount = boundsSrc.count();
                            tclip->boundingCoords = new math::p2d [tclip->boundingCount];

                            for(unsigned int i = 0; i < tclip->boundingCount; i++){
                                const luaObj &curCoord = boundsSrc.get(i);
                                tclip->boundingCoords[i].x = curCoord.get("x");
                                tclip->boundingCoords[i].y = curCoord.get("y");
                            }
                        }

                        auto index = _clips.find((const char *)key);
                        
                        if(index != _clips.end()) {
                            delete index->second;
                        }

                        _clips[(const char *)key] = tclip;
                    }
                    
                    return true;
                });
            }
            else{
                log.msgError("ClipSetResource::loaded %s / %s", _loadPath.data(), src.getLastError());
                _loadingState = ResourceLoadingState::INVALID;
            }
        }

        bool ClipSetResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            return false; // unload binary
        }

        void ClipSetResource::unloaded() {
            for(auto index = _clips.begin(); index != _clips.end(); ++index){
                delete index->second->boundingCoords;
                delete index->second->frames;
                delete index->second;
            }

            _clips.clear();
        }

        const ClipData *ClipSetResource::getClip(const fg::string &name) const {
            auto index = _clips.find(name.data());           //!!

            if(index != _clips.end()){
                return index->second;
            }
            return nullptr;
        }
    }
}


//