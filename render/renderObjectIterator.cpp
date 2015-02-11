
namespace fg {
    namespace object3d {
        RenderObjectIterator::RenderObjectIterator(RenderObjectInterface *root, platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan, float frameTimeMs) :
            _current(root),
            _curComponent(nullptr),
            _platform(platform),
            _resMan(resMan),
            _frameTimeMs(frameTimeMs),
            _componentCount(0),
            _componentIndex(0)
        {
            
        }

        RenderObjectIterator::~RenderObjectIterator() {
        
        }

        object3d::RenderObjectType RenderObjectIterator::type() const {
            return _current->getType();
        }

        RenderObjectConstPtr RenderObjectIterator::object() const {
            return _current;
        }

        RenderObjectComponentPtr RenderObjectIterator::component() const {
            return _curComponent;
        }

        bool RenderObjectIterator::next() {
            while(_componentIndex < _componentCount) {
                _curComponent = _current->getComponentInterface(_componentIndex++);

                if(_curComponent->isResourcesReady(_platform, _resMan)) {
                    return true;
                }
            }

            while(_current = _current->getNext()) {                
                if(_current->isResourcesReady(_platform, _resMan)) {
                    _current->updateCoordinates(_frameTimeMs);
                    _componentIndex = 0;
                    _componentCount = _current->getComponentCount();

                    while(_componentIndex < _componentCount) {
                        _curComponent = _current->getComponentInterface(_componentIndex++);

                        if(_curComponent->isResourcesReady(_platform, _resMan)) {
                            return true;
                        }
                    }
                }
                else {
                    _current->updateCoordinates(_frameTimeMs);
                }
            }

            return false;
        }
    }
}




//