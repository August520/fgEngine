
namespace fg {
    namespace object2d {
        template <typename DOWNTYPE> DisplayObjectPtr::operator DOWNTYPE *() {
            return static_cast <DOWNTYPE *> (_obj);
        }

        template <typename DOWNTYPE> DisplayObjectConstPtr::operator const DOWNTYPE *() {
            return static_cast <const DOWNTYPE *> (_obj);
        }

        template DisplayObjectPtr::operator Sprite2DInterface * ();
        template DisplayObjectPtr::operator DisplayObjectInterface * ();

        template DisplayObjectConstPtr::operator const Sprite2DInterface * ();
        template DisplayObjectConstPtr::operator const DisplayObjectInterface * ();
    }
    
    namespace object3d {
        template <typename DOWNTYPE> RenderObjectPtr::operator DOWNTYPE *() {
            return static_cast <DOWNTYPE *> (_obj);
        }

        template <typename DOWNTYPE> RenderObjectConstPtr::operator const DOWNTYPE *() {
            return static_cast <const DOWNTYPE *> (_obj);
        }

        template <typename DOWNTYPE> RenderObjectComponentPtr::operator const DOWNTYPE *() {
            return static_cast <const DOWNTYPE *> (_component);
        }

        template RenderObjectPtr::operator Particles3DInterface * ();
        template RenderObjectPtr::operator Model3DInterface * ();
        template RenderObjectPtr::operator RenderObjectInterface * ();

        template RenderObjectConstPtr::operator const Particles3DInterface * ();
        template RenderObjectConstPtr::operator const Model3DInterface * ();
        template RenderObjectConstPtr::operator const RenderObjectInterface * ();

        template RenderObjectComponentPtr::operator const Particles3DInterface::EmitterComponentInterface * ();
        template RenderObjectComponentPtr::operator const Model3DInterface::MeshComponentInterface * ();
        template RenderObjectComponentPtr::operator const RenderObjectInterface::ComponentInterface * ();
    }
}