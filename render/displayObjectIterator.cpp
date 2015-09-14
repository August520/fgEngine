
namespace fg {
    namespace object2d {
        DisplayObjectIterator::DisplayObjectIterator(DisplayObjectInterface *root, platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan, float frameTimeMs) : 
            _current(root), 
            _platform(platform),
            _resMan(resMan),
            _frameTimeMs(frameTimeMs) 
        {
        
        }

        DisplayObjectIterator::~DisplayObjectIterator() {
        
        }

        DisplayObjectType DisplayObjectIterator::type() const {
            return _current->getType();
        }

        DisplayObjectConstPtr DisplayObjectIterator::object() const {
            return _current;
        }

        bool DisplayObjectIterator::next() {
            while(_current = _current->getNext()) {
                _current->updateCoordinates(_frameTimeMs, _resMan);

                if(_current->isResourcesReady(_platform, _resMan)) {
                    return true;
                }
            }
            
            return false;
        }
    }
}



//