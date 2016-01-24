
namespace fg {
    namespace object2d {
        DisplayObject::DisplayObject() : _localScale(1.0f, 1.0f) {}

        DisplayObject::DisplayObject(render::EngineSceneCompositionInterface *sceneComposition) : _localScale(1.0f, 1.0f), _sceneComposition(sceneComposition) {

        }

        DisplayObject::~DisplayObject() {
        
        }

        DisplayObjectInterface *DisplayObject::addChild(DisplayObjectInterface *obj) {
            DisplayObjectInterface *objParent = obj->getParent();
            render::EngineSceneCompositionInterface *objSceneComposition = obj->getSceneComposition();
            render::EngineSceneCompositionInterface *mySceneComposition = getSceneComposition();

            if (objParent) {
                obj->callRemoveHandler();
                objParent->removeChild(obj);
            }

            obj->getParent() = this;

            if (objSceneComposition == nullptr && mySceneComposition) {
                mySceneComposition->addDisplayObject(obj);
            }
            if (objSceneComposition && mySceneComposition == nullptr) {
                objSceneComposition->removeDisplayObject(obj);
            }

            _childs.emplace(obj);
            obj->callAddHandler();
            return obj;
        }

        DisplayObjectType DisplayObject::getType() const {
            return _type;
        }

        unsigned DisplayObject::removeChild(DisplayObjectInterface *obj) {
            render::EngineSceneCompositionInterface *mySceneComposition = getSceneComposition();

            obj->callRemoveHandler();
            _childs.erase(obj);

            if (mySceneComposition) {
                mySceneComposition->removeDisplayObject(obj);
            }

            return _childs.size();
        }

        void DisplayObject::removeAllChilds() {
            render::EngineSceneCompositionInterface *mySceneComposition = getSceneComposition();

            for (auto index = std::begin(_childs); index != std::end(_childs); ++index) {
                DisplayObjectInterface *cur = *index;
                cur->callRemoveHandler();

                if (mySceneComposition) {
                    mySceneComposition->removeDisplayObject(cur);
                }
            }

            _childs.clear();
        }

        unsigned DisplayObject::getChildCount() const {
            return unsigned(_childs.size());
        }

        DisplayObjectInterface *&DisplayObject::getParent() {
            return _parent;
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

        render::EngineSceneCompositionInterface *DisplayObject::getSceneComposition() {
            return _parent ? _parent->getSceneComposition() : _sceneComposition;
        }
    }
}