
namespace fg {
    namespace resources {   
        ResourceManager::ResourceManager(const diag::LogInterface &log, platform::EnginePlatformInterface &platform) : _log(log), _platform(platform) {
            _resListReady = 0;
            _elementInProgress = 0;
            _allowLoadingResources = false;
        }

        ResourceManager::~ResourceManager() {
            _resources.foreach([](const fg::string &key, ManagedResourceInterface *ptr){
                delete ptr;                
            });
        }

        void ResourceManager::loadResourcesDir(const fg::string &dir, const callback <void()> &completeCb, bool unloadable) {
            platform::EnginePlatformInterface &p = _platform;
            
            _resListReady++;
            _loadingCompleteCb = completeCb;

            _resListFutures.emplace_back(ResListFuture(unloadable));
            ResListFuture &curFuture = _resListFutures.back();

            _futuresCount++;
            _resListFutures.back().future = std::move(std::async(std::launch::async, [this, &p, dir, &curFuture]() {
                p.fsFormFilesList(dir.data(), curFuture.content);

                _futuresCount--;
                return &curFuture.content;
            }));
        }

        void ResourceManager::loadResourcesList(const fg::string &resList, const callback <void()> &completeCb, bool unloadable) {
            _resListReady++;
            _loadingCompleteCb = completeCb;
            ResourceManager::_resListReadyCallback(resList.data(), unloadable);
        }

        void ResourceManager::unloadResourcesDir(const fg::string &dir) {
        
        }

        void ResourceManager::unloadResourcesList(const fg::string &resList) {
        
        }

        ResourcePtr ResourceManager::createResource(const fg::string &fullpath) {
            char bufName[256];
            char bufExt[16];
            
            _getStringsFromPath(fullpath.data(), bufName, bufExt);
            ManagedResourceInterface *res = _resources.get(bufName);

            if(res == nullptr) {
                res = _factory.createResource(bufExt, fullpath.data(), false);
                res->setLoadingState(ResourceLoadingState::CONSTRUCTED);

                if(res) {
                    _resources.add(bufName, res);
                    return res;
                }
            }
            else {
                return res;
            }

            return (ManagedResourceInterface *)nullptr;
        }

        ResourcePtr ResourceManager::getResource(const fg::string &path) const {
            return _resources.get(path);
        }

        void ResourceManager::init() {
            _resListReady = 0;
            _elementInProgress = 0;
            _allowLoadingResources = true;
        }
        
        void ResourceManager::update(float frameTimeMs) {
            if(_allowLoadingResources == false) {
                return;
            }

            if(_resListFutures.size()) {
                if(_resListFutures.front().future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) { //
                    std::string *result = _resListFutures.front().future.get();
                    
                    _resListReadyCallback(result->c_str(), _resListFutures.front().isUnloadable);
                    _resListFutures.pop_front();
                }
            }

            for(unsigned i = 0; i < FG_RES_UPDATING_MAX; i++) {
                ManagedResourceInterface *curResource = _resources.nextItem();

                if(curResource) {
                    if(curResource->getLoadingState() == ResourceLoadingState::NOTLOADED){
                        if(curResource->getUnusedTimeMs() <= FG_UNLOAD_TIME && _resourceLoadingFutures.size() < FG_RES_LOADING_MAX) {
                            curResource->setLoadingState(ResourceLoadingState::LOADING);

                            platform::EnginePlatformInterface &platform = _platform;
                            const fg::diag::LogInterface &log = _log;

                            _futuresCount++;
                            _resourceLoadingFutures.push_back(std::async(std::launch::async, [this, curResource, &platform, &log](){
                                void      *binaryData = nullptr;
                                unsigned  binarySize = 0;
                                
                                if(platform.fsLoadFile(curResource->getFilePath().data(), &binaryData, &binarySize)){
                                    curResource->setBinary(binaryData, binarySize);
                                    curResource->loaded(log);
                                    curResource->setLoadingState(ResourceLoadingState::LOADED);
                                }
                                else{
                                    curResource->setLoadingState(ResourceLoadingState::INVALID);
                                }

                                _futuresCount--;
                                return curResource;
                            }));
                        }
                    }
                    else if(curResource->getLoadingState() == ResourceLoadingState::CONSTRUCTED){
                        #ifdef FG_RESOURCE_UNLOADABLE
                        if(curResource->unloadable() && curResource->getUnusedTimeMs() > FG_UNLOAD_TIME){
                            curResource->unloaded();
                            curResource->setLoadingState(ResourceLoadingState::NOTLOADED);
                            _log.msgInfo("%s unloaded", curResource->getFilePath().data());
                        }
                        else{
                            curResource->setUnusedTimeMs(curResource->getUnusedTimeMs() + unsigned(frameTimeMs));
                        }
                        #endif
                    }

                    if(curResource->getSavingState() == ResourceSavingState::NEEDSAVE) {
                        platform::EnginePlatformInterface &platform = _platform;
                        const fg::diag::LogInterface &log = _log;

                        curResource->setSavingState(ResourceSavingState::SAVING);
                        _resourceSavingFutures.push_back(std::async(std::launch::async, [this, curResource, &platform, &log]() {
                            void      *binaryData = nullptr;
                            unsigned  binarySize = 0;

                            curResource->save(&binaryData, &binarySize);

                            if(platform.fsSaveFile(curResource->getFilePath().data(), binaryData, binarySize)) {
                                curResource->setSavingState(ResourceSavingState::SAVED);
                            }
                            else {
                                curResource->setSavingState(ResourceSavingState::INVALID);
                            }

                            delete (char *)binaryData;
                            return curResource;
                        }));
                    }
                }
            }

            if(_resourceLoadingFutures.size()) {
                if(_resourceLoadingFutures.front().wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) { //
                    ManagedResourceInterface *curResource = _resourceLoadingFutures.front().get();
                    _resourceLoadingFutures.pop_front();

                    if(curResource->getLoadingState() == ResourceLoadingState::LOADED) {
                        _log.msgInfo("%s loaded", curResource->getFilePath().data());

                        if(_platform.isInited()) {
                            if(curResource->constructed(_log, _platform) == false) {
                                curResource->freeBinary();
                            }
                        }

                        // errors can occur in 'constucted' method (resource must set state to INVALID)
                        //
                        if(curResource->getLoadingState() != ResourceLoadingState::INVALID) {  
                            _log.msgInfo("%s constructed", curResource->getFilePath().data());
                            curResource->setLoadingState(ResourceLoadingState::CONSTRUCTED);
                        }
                    }
                    else {
                        _log.msgWarning("ResourceManager::_update / file %s not found or resource invalid", curResource->getFilePath().data());
                    }

                    _elementInProgress--;
                }
            }

            if(_resourceSavingFutures.size()) {
                if(_resourceSavingFutures.front().wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) { //
                    ManagedResourceInterface *curResource = _resourceSavingFutures.front().get();
                    _resourceSavingFutures.pop_front();

                    if(curResource->getSavingState() == ResourceSavingState::INVALID) {
                        _log.msgWarning("ResourceManager::_update / file %s can not be saved", curResource->getFilePath().data());
                    }
                }
            }

            if(_elementInProgress == 0 && _resListReady == 0) {
                if(_loadingCompleteCb.isBinded()) {
                    auto tcall = _loadingCompleteCb;
                    _loadingCompleteCb = nullptr;
                    tcall();
                }
            }
        }

        void ResourceManager::destroy() {
            _allowLoadingResources = false;

            while(_futuresCount != 0) {            
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            _resListFutures.clear();
            _resourceLoadingFutures.clear();

            _resources.foreach([](const fg::string &key, ManagedResourceInterface *ptr) {
                delete ptr;
            });

            _resources.clear();
        }

        void ResourceManager::_resListReadyCallback(const char *resList, bool unloadable) {
            int  i = 0, tlen;
            char bufName[256];
            char bufExt[16];

            while(resList[i] != 0) {
                tlen = _getStringsFromPath(resList + i, bufName, bufExt);
                bool created = true;
                
                if(_resources.get(bufName) == nullptr) {
                    const char *curResName = resList + i;
                    ManagedResourceInterface *r = _factory.createResource(bufExt, curResName, unloadable); 

                    if(r != nullptr) {
                        _resources.add(bufName, r);
                    }
                    else {
                        created = false;
                        _log.msgWarning("ResourceManager::_resListReadyCallback / unknown resource extension %s", bufExt);
                    }
                }
                else {
                    created = false;
                    _log.msgInfo("ResourceManager::_resListReadyCallback / resource already in resMan %s", bufName);
                }

                if(created) {
                    _elementInProgress++;
                }

                i += tlen;
                while(resList[i] == '\n' || resList[i] == '\r') i++;
            }

            _resListReady--;
        }

        int ResourceManager::_getStringsFromPath(const char *path, char *outName, char *outExtension) {
            int offset = 0;

            while(path[offset] != 0 && path[offset] != '\n' && path[offset] != '\r') offset++;
            int len = offset--;
            int extStart = 0, nameStart = 0;
            int i;

            #ifdef FG_RESOURCE_LONGNAME

            while(offset >= 0){
                if(path[offset] == '.' && !extStart){
                    extStart = offset + 1;
                    break;
                }
                offset--;
            }

            #else

            while(offset >= 0){
                if(path[offset] == '.' && !extStart) extStart = offset + 1;
                if(path[offset] == '\\' || path[offset] == '/'){
                    nameStart = offset + 1;
                    break;
                }
                offset--;
            }

            #endif

            for(i = nameStart; i < len; i++){
                outName[i - nameStart] = (path[i] == '\\' ? '/' : path[i]);
            }
            outName[i - nameStart] = 0;

            for(i = extStart; i < len; i++) outExtension[i - extStart] = path[i];
            outExtension[i - extStart] = 0;

            return len;
        }

    }
}
