
namespace fg {
    object2d::DisplayObjectPtr GameAPI::create(object2d::DisplayObjectType type) {
        if(type == object2d::DisplayObjectType::NONE) {
            return new object2d::DisplayObject ();
        }
        if(type == object2d::DisplayObjectType::SPRITE) {
            return static_cast <object2d::Sprite2DInterface *> (new object2d::Sprite2D()); //
        }
        return nullptr;
    }

    object3d::RenderObjectPtr GameAPI::create(object3d::RenderObjectType type) {
        if(type == object3d::RenderObjectType::NONE) {
            return new object3d::RenderObject ();
        }
        if(type == object3d::RenderObjectType::MODEL) {
            return static_cast <object3d::Model3DInterface *> (new object3d::Model3D ());
        }
        if(type == object3d::RenderObjectType::PARTICLES) {
            return static_cast <object3d::Particles3DInterface *> (new object3d::Particles3D());
        }
        return nullptr;
    }

    render::CameraInterface &GameAPI::getCamera() {
        return *_engine._gameCamera;
    }

    GameAPI::GameAPI(Engine &eng) : _engine(eng), resources(eng._resMan), input(eng._input), root2D(eng._root2D), root3D(eng._root3D) {

    }
    
    GameAPI::~GameAPI() {

    }
}