
namespace fg {
    namespace object2d {
        DisplayObject::DisplayObject() : 
            _type(DisplayObjectType::NONE),
            _parent(nullptr),
            _next(nullptr),
            _back(nullptr),
            _localScale(1.0f, 1.0f),
            _rotationInDegrees(0.0f),
            _zCoord(0.0f), 
            _alpha(1.0f),
            _visible(true),
            _resolutionDependent(false)
        {
        
        }

        DisplayObject::~DisplayObject() {
        
        }

        DisplayObjectInterface *DisplayObject::addChild(DisplayObjectInterface *obj) {
            DisplayObjectInterface *objParent = obj->getParent();

            if(objParent) {
                objParent->removeChild(obj);
            }

            obj->getParent() = this;
            DisplayObjectInterface *last = obj;

            while(last->getChildCount()) {
                last = last->getChildBack();
            }

            if(_childs.size() == 0) {
                last->getNext() = _next;
                _next = obj;
                obj->getBack() = this;
            }
            else {
                DisplayObjectInterface *tchild = this;

                while(tchild->getChildCount()) {
                    tchild = tchild->getChildBack();
                }

                last->getNext() = tchild->getNext();
                tchild->getNext() = obj;
                obj->getBack() = tchild;
            }

            _childs.push_back(obj);

            obj->callAddHandler();
            return obj;
        }

        DisplayObjectType DisplayObject::getType() const {
            return _type;
        }

        unsigned DisplayObject::removeChild(DisplayObjectInterface *obj) {
            for(unsigned i = 0; i < _childs.size(); i++) {
                if(_childs[i] == obj) {
                    removeChild(i);
                    break;
                }
            }
            return _childs.size();
        }

        unsigned DisplayObject::removeChild(unsigned index) {
            if(index < _childs.size()) {
                DisplayObjectInterface *obj = _childs[index];
                obj->getParent() = nullptr;

                DisplayObjectInterface *last = obj;
                while(last->getChildCount()) {
                    last = last->getChildBack();
                }

                obj->getBack()->getNext() = last->getNext();
                if(last->getNext()) {
                    last->getNext()->getBack() = obj->getBack();
                }

                last->getNext() = nullptr;
                obj->getBack() = nullptr;

                // TODO: optimize
                _childs.erase(_childs.begin() + index);
                obj->callRemoveHandler();
            }

            return _childs.size();
        }

        void DisplayObject::removeAllChilds() {
            while(removeChild(unsigned(0)));
        }

        unsigned DisplayObject::getChildCount() const {
            return unsigned(_childs.size());
        }

        DisplayObjectInterface *DisplayObject::getChildAt(unsigned index) const {
            return _childs[index];
        }
        
        DisplayObjectInterface *DisplayObject::getChildBack() const {
            return _childs.back();
        }

        DisplayObjectInterface *&DisplayObject::getParent() {
            return _parent;
        }

        DisplayObjectInterface *&DisplayObject::getNext() {
            return _next;
        }

        DisplayObjectInterface *&DisplayObject::getBack() {
            return _back;
        }

        void DisplayObject::setAddHandler(const callback <void()> &cb) {
            _addHandler = cb;
        }

        void DisplayObject::setUpdateHandler(const callback <void(float)> &cb) {
            _updateHandler = cb;
        }

        void DisplayObject::setRemoveHandler(const callback <void()> &cb) {
            _removeHandler = cb;
        }

        void DisplayObject::callAddHandler() {
            if(_addHandler.isBinded()) {
                _addHandler();
            }
        }

        void DisplayObject::callUpdateHandler(float frameTimeMs) {
            if(_updateHandler.isBinded()) {
                _updateHandler(frameTimeMs);
            }
        }

        void DisplayObject::callRemoveHandler() {
            if(_removeHandler.isBinded()) {
                _removeHandler();
            }
        }

        void DisplayObject::setPosition(float x, float y) {
            _localTransform._31 = x;
            _localTransform._32 = y;
        }

        void DisplayObject::setPosition(const math::p2d &pos) {
            setPosition(pos.x, pos.y);
        }

        void DisplayObject::setRotation(float degrees) {
            float tangle = degrees / 180.0f * float(M_PI);

            _localTransform._11 = _localTransform._22 = cosf(tangle);
            _localTransform._12 = sinf(tangle);
            _localTransform._21 = -_localTransform._12;
            _localTransform._33 = 1.0f;

            _rotationInDegrees = degrees;
        }

        void DisplayObject::setRotation(const math::p2d &dir) {
            float tangle = math::p2d(0.0f, -1.0f).angleTo(dir);
            
            if(dir.x <= 0.0f) {
                tangle = -tangle;
            }

            _localTransform._11 = _localTransform._22 = cosf(tangle);
            _localTransform._12 = sinf(tangle);
            _localTransform._21 = -_localTransform._12;
            _localTransform._33 = 1.0f;

            _rotationInDegrees = tangle * 180.0f / float(M_PI);
        }

        void DisplayObject::setScaling(float sclx, float scly) {
            _localScale.x = sclx;
            _localScale.y = scly;
        }

        void DisplayObject::setScaling(const math::p2d &scale) {
            _localScale = scale;
        }

        void DisplayObject::appendPosition(float x, float y) {
            _localTransform._31 += x;
            _localTransform._32 += y;
        }

        void DisplayObject::appendPosition(const math::p2d &posIncrement) {
            appendPosition(posIncrement.x, posIncrement.y);
        }

        void DisplayObject::appendRotation(float degrees) {
            _rotationInDegrees += degrees;
            setRotation(_rotationInDegrees);
        }

        void DisplayObject::setTransform(const math::m3x3 &trasformMatrix) {
            _localTransform = trasformMatrix;
        }

        void DisplayObject::setVisible(bool visible) {
            _visible = visible;
        }

        void DisplayObject::setResolutionDependency(bool value) {
            _resolutionDependent = value;
        }

        const math::m3x3 &DisplayObject::getFullTransform() const {
            return _fullTransform;
        }

        const math::m3x3 &DisplayObject::getLocalTransform() const {
            return _localTransform;
        }

        const math::p2d &DisplayObject::getPosition() const {
            return *(const math::p2d *)&_localTransform._31;        
        }

        const math::p2d &DisplayObject::getScaling() const {
            return _localScale;
        }

        void DisplayObject::setZ(float z) {
            _zCoord = z;
        }

        float DisplayObject::getZ() const {
            return _zCoord;
        }

        void DisplayObject::setAlpha(float alpha) {
            _alpha = alpha;
        }

        float DisplayObject::getAlpha() const {
            return _alpha;
        }

        float DisplayObject::getRotation() const {
            return _rotationInDegrees;
        }

        bool DisplayObject::isVisible() const {
            return _visible;
        }

        bool DisplayObject::isResolutionDepended() const {
            return _resolutionDependent;
        }

        bool DisplayObject::hitTestPoint(const math::p2d &point, const math::p2d &dpiFactor) const {
            return false;
        }

        bool DisplayObject::hitTestLine(const math::p2d &point, const math::p2d &dpiFactor) const {
            return false;
        }

        void DisplayObject::updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) {
            if(_updateHandler.isBinded()) {
                _updateHandler(frameTimeMs);
            }

            math::m3x3 fullLocalTransform = math::m3x3(_localScale.x, 0.0f, 0.0f, 0.0f, _localScale.y, 0.0f, 0.0f, 0.0f, 1.0f) * _localTransform;

            if(_parent) {
                _fullTransform = fullLocalTransform * _parent->getFullTransform();
            }
            else {
                _fullTransform = fullLocalTransform;
            }
        }

        bool DisplayObject::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            return false;
        }
    }
}