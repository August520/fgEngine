
namespace fg {
    namespace object2d {
        struct DisplayObjectPtr {
            DisplayObjectInterface *_obj;
            DisplayObjectPtr(DisplayObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator DOWNTYPE *();
        };

        struct DisplayObjectConstPtr {
            const DisplayObjectInterface *_obj;
            DisplayObjectConstPtr(const DisplayObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator const DOWNTYPE *();
        };

        class DisplayObjectIteratorInterface {
        public:
            virtual ~DisplayObjectIteratorInterface() {}

            virtual DisplayObjectType      type() const = 0;
            virtual DisplayObjectConstPtr  object() const = 0;

            virtual bool next() = 0;
        };
    }

    namespace object3d {
        struct RenderObjectPtr {
            RenderObjectInterface *_obj;
            RenderObjectPtr(RenderObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator DOWNTYPE *();
        };

        struct RenderObjectConstPtr {
            const RenderObjectInterface *_obj;
            RenderObjectConstPtr(const RenderObjectInterface *obj) : _obj(obj) {}

            template <typename DOWNTYPE> operator const DOWNTYPE *();
        };

        struct RenderObjectComponentPtr {
            const RenderObjectInterface::ComponentInterface *_component;
            RenderObjectComponentPtr(const RenderObjectInterface::ComponentInterface *component) : _component(component) {}

            template <typename DOWNTYPE> operator const DOWNTYPE *();
        };

        class RenderObjectIteratorInterface {
        public:
            virtual ~RenderObjectIteratorInterface() {}

            virtual RenderObjectType          type() const = 0;
            virtual RenderObjectConstPtr      object() const = 0;
            virtual RenderObjectComponentPtr  component() const = 0;

            virtual bool next() = 0;
        };
    }

}