
namespace fg {
    namespace resources {
        Resource::Resource(const char *path, bool unloadable) : 
            _binaryData(nullptr), 
            _binarySize(0), 
            _loadingState(ResourceLoadingState::NOTLOADED),
            _savingState(ResourceSavingState::SAVED),
            _unloadable(unloadable) 
        {
            int i = 0;
            
            while(path[i] != 0 && path[i] != '\n' && path[i] != '\r') {
                i++;
            }

            _loadPath = fg::string(path, i);
            _unusedTime = 0;
        }

        Resource::~Resource() {
            if(_binaryData != nullptr) {
                delete (char *)_binaryData;
            }
        }
        
        bool Resource::valid() const {
            _unusedTime = 0;
            return _loadingState == ResourceLoadingState::CONSTRUCTED;
        }

        bool Resource::unloadable() const {
            return _unloadable;
        }
        
        const fg::string &Resource::getFilePath() const {
            return _loadPath;
        }
        
        ResourceLoadingState Resource::getLoadingState() const {
            return _loadingState;
        }

        ResourceSavingState Resource::getSavingState() const {
            return _savingState;
        }

        unsigned Resource::getUnusedTimeMs() const {
            return _unusedTime;
        }

        void Resource::setUnusedTimeMs(unsigned value) {
            _unusedTime = value;
        }
        
        void Resource::setLoadingState(ResourceLoadingState state) {
            _loadingState = state;
        }

        void Resource::setSavingState(ResourceSavingState state) {
            _savingState = state;
        }

        void Resource::setBinary(void *binaryData, unsigned binarySize) {
            _binaryData = binaryData;
            _binarySize = binarySize;
        }

        void Resource::freeBinary() {
            delete (char *)_binaryData;
            _binaryData = nullptr;
            _binarySize = 0;
        }

        bool Resource::commit() {
            return true;
        }
    }
}


