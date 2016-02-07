
namespace fg {
    namespace object2d {
        struct DisplayObjectPtr {
            DisplayObjectInterface *_obj;
            DisplayObjectPtr(DisplayObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator DOWNTYPE *() {
                return static_cast <DOWNTYPE *> (_obj);
            }
        };

        struct DisplayObjectConstPtr {
            const DisplayObjectInterface *_obj;
            DisplayObjectConstPtr(const DisplayObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator const DOWNTYPE *() {
                return static_cast <const DOWNTYPE *> (_obj);
            }
        };
    }

    namespace object3d {
        struct RenderObjectPtr {
            RenderObjectInterface *_obj;
            RenderObjectPtr(RenderObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator DOWNTYPE *() {
                return static_cast <DOWNTYPE *> (_obj);
            }
        };

        struct RenderObjectConstPtr {
            const RenderObjectInterface *_obj;
            RenderObjectConstPtr(const RenderObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator const DOWNTYPE *() {
                return static_cast <const DOWNTYPE *> (_obj);
            }
        };

        struct RenderObjectComponentPtr {
            const RenderObjectComponentInterface *_component;
            RenderObjectComponentPtr(const RenderObjectComponentInterface *component) : _component(component) {}

            template <typename DOWNTYPE> operator const DOWNTYPE *() {
                return static_cast <const DOWNTYPE *> (_component);
            }
        };
    }

}