
namespace fg {
    object2d::DisplayObjectPtr GameAPI::create(object2d::DisplayObjectType type) {
        switch (type) {
            case fg::object2d::DisplayObjectType::NONE:
                return new object2d::DisplayObject();
            case fg::object2d::DisplayObjectType::SPRITE:
                return static_cast <object2d::Sprite2DInterface *> (new object2d::Sprite2D()); //
            case fg::object2d::DisplayObjectType::TEXTFIELD:
                return static_cast <object2d::TextFieldInterface *> (new object2d::TextField()); //
            default:
                return nullptr;
        }
    }

    object3d::RenderObjectPtr GameAPI::create(object3d::RenderObjectType type) {
        switch (type) {
            case fg::object3d::RenderObjectType::NONE:
                return new object3d::RenderObject();
            case fg::object3d::RenderObjectType::MODEL:
                return static_cast <object3d::Model3DInterface *> (new object3d::Model3D());
            case fg::object3d::RenderObjectType::PARTICLES:
                return static_cast <object3d::Particles3DInterface *> (new object3d::Particles3D());
            case fg::object3d::RenderObjectType::LIGHT:
                return static_cast <object3d::PointLightInterface *> (new object3d::PointLight());
            default:
                return nullptr;
        }
    }

    float GameAPI::getCoordSystemWidth() const {
        return _engine.getCoordSystemWidth();
    }

    float GameAPI::getCoordSystemHeight() const {
        return _engine.getCoordSystemHeight();
    }

    float GameAPI::getDPIFactorX() const {
        return _engine.getCoordSystemDPIFactorX();
    }

    float GameAPI::getDPIFactorY() const {
        return _engine.getCoordSystemDPIFactorY();    
    }

    math::p2d GameAPI::getCoordSystemDimension() const {
        return math::p2d(_engine.getCoordSystemWidth(), _engine.getCoordSystemHeight());
    }

    math::p2d GameAPI::getDPIFactor() const {
        return math::p2d(_engine.getCoordSystemDPIFactorX(), _engine.getCoordSystemDPIFactorY());
    }
    
    render::CameraInterface &GameAPI::getCamera() {
        return *_engine._gameCamera;
    }

    GameAPI::GameAPI(Engine &eng) : _engine(eng), resources(eng._resMan), input(eng._input), root2D(eng._root2D), root3D(eng._root3D) {

    }
    
    GameAPI::~GameAPI() {

    }
}