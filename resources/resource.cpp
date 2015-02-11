
namespace fg {
    namespace resources {
        Resource::Resource(const char *path, bool unloadable) : _binaryData(nullptr), _binarySize(0), _state(ResourceState::NOTLOADED), _unloadable(unloadable) {
            int i = 0;
            
            while(path[i] != 0 && path[i] != '\n' && path[i] != '\r') {
                i++;
            }

            _loadPath = fg::string(path, i);
            _unusedTime = 0;
        }

        Resource::~Resource() {
            if(_binaryData != nullptr) {
                delete[] _binaryData;
            }
        }
        
        bool Resource::valid() const {
            _unusedTime = 0;
            return _state == ResourceState::CONSTRUCTED;
        }

        bool Resource::unloadable() const {
            return _unloadable;
        }
        
        const fg::string &Resource::getFilePath() const {
            return _loadPath;
        }

        ResourceState Resource::getState() const {
            return _state;
        }

        unsigned Resource::getUnusedTimeMs() const {
            return _unusedTime;
        }

        void Resource::setUnusedTimeMs(unsigned value) {
            _unusedTime = value;
        }
        
        void Resource::setState(ResourceState state) {
            _state = state;
        }

        void Resource::setBinary(void *binaryData, unsigned binarySize) {
            _binaryData = binaryData;
            _binarySize = binarySize;
        }

        void Resource::freeBinary() {
            delete _binaryData;
            _binaryData = nullptr;
            _binarySize = 0;
        }

    }
}


