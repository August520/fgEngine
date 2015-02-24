
namespace fg {
    namespace object3d {
        RenderObject::RenderObject() : 
            _type(RenderObjectType::NONE),
            _parent(nullptr),
            _next(nullptr),
            _back(nullptr),
            _localScale(1.0f, 1.0f, 1.0f),
            _visible(true) 
        {
        
        }

        RenderObject::~RenderObject() {
        
        }

        RenderObjectInterface *RenderObject::addChild(RenderObjectInterface *obj) {
            RenderObjectInterface *objParent = obj->getParent();

            if(objParent) {
                objParent->removeChild(obj);
            }

            obj->getParent() = this;
            RenderObjectInterface *last = obj;

            while(last->getChildCount()) {
                last = last->getChildBack();
            }

            if(_childs.size() == 0) {
                last->getNext() = _next;
                _next = obj;
                obj->getBack() = this;
            }
            else {
                RenderObjectInterface *tchild = this;

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

        RenderObjectType RenderObject::getType() const {
            return _type;
        }

        unsigned RenderObject::removeChild(RenderObjectInterface *obj) {
            for(unsigned i = 0; i < _childs.size(); i++) {
                if(_childs[i] == obj) {
                    removeChild(i);
                    break;
                }
            }
            return _childs.size();
        }

        unsigned RenderObject::removeChild(unsigned index) {
            if(index < _childs.size()) {
                RenderObjectInterface *obj = _childs[index];
                obj->getParent() = nullptr;

                RenderObjectInterface *last = obj;
                while(last->getChildCount()) {
                    last = last->getChildBack();
                }

                obj->getBack()->getNext() = last->getNext();
                if(last->getNext()) {
                    last->getNext()->getBack() = obj->getBack();
                }

                last->getNext() = nullptr;
                obj->getBack() = nullptr;

                _childs[index] = _childs.back();
                _childs.pop_back();

                obj->callRemoveHandler();
            }

            return _childs.size();
        }

        void RenderObject::removeAllChilds() {
            while(removeChild(unsigned(0)));
        }

        unsigned RenderObject::getChildCount() const {
            return unsigned(_childs.size());
        }

        RenderObjectInterface *RenderObject::getChildAt(unsigned index) const {
            return _childs[index];
        }

        RenderObjectInterface *RenderObject::getChildBack() const {
            return _childs.back();
        }

        RenderObjectInterface *&RenderObject::getParent() {
            return _parent;
        }

        RenderObjectInterface *&RenderObject::getNext() {
            return _next;
        }

        RenderObjectInterface *&RenderObject::getBack() {
            return _back;
        }

        void RenderObject::setAddHandler(const callback <void()> &cb) {
            _addHandler = cb;
        }

        void RenderObject::setRemoveHandler(const callback <void()> &cb) {
            _removeHandler = cb;
        }

        void RenderObject::callAddHandler() {
            if(_addHandler.isBinded()) {
                _addHandler();
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

        void RenderObject::setRotation(float yawRad, float pitchRad, float rollRad) {
            float cosYaw = cosf(rollRad);
            float cosPitch = cosf(yawRad);
            float cosRoll = cosf(pitchRad);

            float sinYaw = sinf(rollRad);
            float sinPitch = sinf(yawRad);
            float sinRoll = sinf(pitchRad);
        
            _localTransform._11 = cosPitch * cosYaw,
            _localTransform._12 = -sinYaw * cosPitch,
            _localTransform._13 = sinPitch,
    
            _localTransform._21 = cosYaw * sinPitch * sinRoll + sinYaw * cosRoll,
            _localTransform._22 = -sinYaw * sinPitch * sinRoll + cosYaw * cosRoll,
            _localTransform._23 = -cosPitch * sinRoll,
        
            _localTransform._31 = -cosYaw * sinPitch * cosRoll + sinYaw * sinRoll,
            _localTransform._32 = sinYaw * sinPitch * cosRoll + cosYaw * sinRoll,
            _localTransform._33 = cosPitch * cosRoll;

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

            _localTransform.toQuaternion(_localRotation);
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

        void RenderObject::setColor(float r, float g, float b, float a) {
            _rgba = fg::color(r, g, b, a);
        }

        void RenderObject::setColor(const fg::color &rgba) {
            _rgba = rgba;
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

        const fg::color &RenderObject::getColor() const {
            return _rgba;
        }

        bool RenderObject::isVisible() const {
            return _visible;
        }

        void RenderObject::updateCoordinates(float frameTimeMs) {
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

        RenderObjectInterface::ComponentInterface *RenderObject::getComponentInterface(unsigned index) {
            return nullptr;
        }
    }
}





//