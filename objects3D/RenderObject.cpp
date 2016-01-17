
namespace fg {
    namespace object3d {
        RenderObject::RenderObject() : _localScale(1.0f, 1.0f, 1.0f) {
        
        }

        RenderObject::RenderObject(render::EngineSceneCompositionInterface *sceneComposition) : _localScale(1.0f, 1.0f, 1.0f), _sceneComposition(sceneComposition) {

        }

        RenderObject::~RenderObject() {
        
        }

        RenderObjectInterface *RenderObject::addChild(RenderObjectInterface *obj) {
            RenderObjectInterface *objParent = obj->getParent();
            render::EngineSceneCompositionInterface *objSceneComposition = obj->getSceneComposition();

            if (objParent) {
                obj->callRemoveHandler();
                objParent->removeChild(obj);
            }

            if (objSceneComposition == nullptr && _sceneComposition) {
                _sceneComposition->addRenderObject(obj);
            }
            if (objSceneComposition && _sceneComposition == nullptr) {
                objSceneComposition->removeRenderObject(obj);
            }

            _childs.emplace(obj);
            obj->callAddHandler();
            return obj;
        }

        RenderObjectType RenderObject::getType() const {
            return _type;
        }

        unsigned RenderObject::removeChild(RenderObjectInterface *obj) {
            obj->callRemoveHandler();
            _childs.erase(obj);

            if (_sceneComposition) {
                _sceneComposition->removeRenderObject(obj);
            }

            return _childs.size();
        }

        void RenderObject::removeAllChilds() {
            for (auto index = std::begin(_childs); index != std::end(_childs); ++index) {
                RenderObjectInterface *cur = *index;
                cur->callRemoveHandler();
                
                if (_sceneComposition) {
                    _sceneComposition->removeRenderObject(cur);
                }
            }

            _childs.clear();
        }

        unsigned RenderObject::getChildCount() const {
            return unsigned(_childs.size());
        }

        RenderObjectInterface *&RenderObject::getParent() {
            return _parent;
        }

        void RenderObject::setAddHandler(const callback <void()> &cb) {
            _addHandler = cb;
        }
        
        void RenderObject::setUpdateHandler(const callback <void (float)> &cb) {
            _updateHandler = cb;
        }

        void RenderObject::setRemoveHandler(const callback <void()> &cb) {
            _removeHandler = cb;
        }

        void RenderObject::callAddHandler() {
            if(_addHandler.isBinded()) {
                _addHandler();
            }
        }

        void RenderObject::callUpdateHandler(float frameTimeMs) {
            if(_updateHandler.isBinded()) {
                _updateHandler(frameTimeMs);
            }
        }

        void RenderObject::callRemoveHandler() {
            if(_removeHandler.isBinded()) {
                _removeHandler();
            }
        }

        void RenderObject::setPosition(float posX, float posY, float posZ) {
            _localTransform._41 = _localPosition.x = posX;
            _localTransform._42 = _localPosition.y = posY;
            _localTransform._43 = _localPosition.z = posZ;
        }

        void RenderObject::setPosition(const math::p3d &position) {
            setPosition(position.x, position.y, position.z);
        }

        void RenderObject::setScaling(float sclX, float sclY, float sclZ) {
            _localScale.x = sclX;
            _localScale.y = sclY;
            _localScale.z = sclZ;
            (*(math::p3d *)&_localTransform._11).toLength(sclX);
            (*(math::p3d *)&_localTransform._21).toLength(sclY);
            (*(math::p3d *)&_localTransform._31).toLength(sclZ);
        }

        void RenderObject::setScaling(const math::p3d &scale) {
            setScaling(scale.x, scale.y, scale.z);
        }

        void RenderObject::setRotation(float angleRad, const math::p3d &axis) {
            _localRotation.rotationAxis(angleRad, axis);
            _localRotation.toMatrix(_localTransform);

            _localTransform._11 *= _localScale.x;
            _localTransform._12 *= _localScale.x;
            _localTransform._13 *= _localScale.x;
    
            _localTransform._21 *= _localScale.y;
            _localTransform._22 *= _localScale.y;
            _localTransform._23 *= _localScale.y;
    
            _localTransform._31 *= _localScale.z;
            _localTransform._32 *= _localScale.z;
            _localTransform._33 *= _localScale.z;

            _localTransform._41 = _localPosition.x;
            _localTransform._42 = _localPosition.y;
            _localTransform._43 = _localPosition.z;
        }

        void RenderObject::setRotation(const math::quat &rotation) {
            _localRotation = rotation;
            _localRotation.toMatrix(_localTransform);

            _localTransform._11 *= _localScale.x;
            _localTransform._12 *= _localScale.x;
            _localTransform._13 *= _localScale.x;

            _localTransform._21 *= _localScale.y;
            _localTransform._22 *= _localScale.y;
            _localTransform._23 *= _localScale.y;

            _localTransform._31 *= _localScale.z;
            _localTransform._32 *= _localScale.z;
            _localTransform._33 *= _localScale.z;

            _localTransform._41 = _localPosition.x;
            _localTransform._42 = _localPosition.y;
            _localTransform._43 = _localPosition.z;
        }

        void RenderObject::setTransform(const math::m4x4 &transform) {
            _localTransform = transform;
            _localPosition = *(math::p3d *)&_localTransform._41;
            _localTransform.toQuaternion(_localRotation);
            _localScale.x = ((math::p3d *)&_localTransform._11)->length();
            _localScale.y = ((math::p3d *)&_localTransform._21)->length();
            _localScale.z = ((math::p3d *)&_localTransform._31)->length();
        }

        void RenderObject::setVisible(bool visible) {
            _visible = visible;
        }

        void RenderObject::appendPosition(float xInc, float yInc, float zInc) {
            _localTransform._41 = _localPosition.x += xInc;
            _localTransform._42 = _localPosition.y += yInc;
            _localTransform._43 = _localPosition.z += zInc;
        }

        void RenderObject::appendPosition(const math::p3d &posInc) {
            appendPosition(posInc.x, posInc.y, posInc.z);
        }

        void RenderObject::appendRotation(const math::quat &rotInc) {
            _localRotation = _localRotation * rotInc;
            _localRotation.toMatrix(_localTransform);

            _localTransform._11 *= _localScale.x;
            _localTransform._12 *= _localScale.x;
            _localTransform._13 *= _localScale.x;

            _localTransform._21 *= _localScale.y;
            _localTransform._22 *= _localScale.y;
            _localTransform._23 *= _localScale.y;

            _localTransform._31 *= _localScale.z;
            _localTransform._32 *= _localScale.z;
            _localTransform._33 *= _localScale.z;

            _localTransform._41 = _localPosition.x;
            _localTransform._42 = _localPosition.y;
            _localTransform._43 = _localPosition.z;
        }

        const math::p3d &RenderObject::getPosition() const {
            return _localPosition;
        }

        const math::p3d &RenderObject::getScaling() const {
            return _localScale;
        }

        const math::quat &RenderObject::getRotation() const {
            return _localRotation;
        }

        const math::m4x4 &RenderObject::getTransform() const {
            return _localTransform;
        }

        const math::m4x4 &RenderObject::getFullTransform() const {
            return _fullTransform;
        }

        bool RenderObject::isVisible() const {
            return _visible;
        }

        void RenderObject::updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) {
            if(_updateHandler.isBinded()) {
                _updateHandler(frameTimeMs);
            }

            if(_parent) {
                _fullTransform = _localTransform * _parent->getFullTransform();
            }
            else {
                _fullTransform = _localTransform;
            }
        }

        bool RenderObject::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            return false;
        }

        unsigned RenderObject::getComponentCount() const {
            return 0;
        }

        RenderObjectComponentInterface *RenderObject::getComponentInterface(unsigned index) {
            return nullptr;
        }

        render::EngineSceneCompositionInterface *RenderObject::getSceneComposition() {
            return _parent ? _parent->getSceneComposition() : _sceneComposition;
        }
    }
}





//