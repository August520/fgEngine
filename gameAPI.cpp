
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
    
    template <typename F, typename ...ARGS> void GameAPI::postponedDispatch(F func, ARGS... args) {
        _engine.postponedDispatch(func, args...);
    }
        
    render::CameraInterface &GameAPI::getCamera() {
        return *_engine._gameCamera;
    }

    GameAPI::GameAPI(Engine &eng) : _engine(eng), resources(eng._resMan), input(eng._input), root2D(eng._root2D), root3D(eng._root3D) {

    }
    
    GameAPI::~GameAPI() {

    }
}