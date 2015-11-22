

namespace fg {
    struct GameAPI {        
        resources::ResourceManagerInterface  &resources;
        input::InputManagerInterface         &input;
        
        object2d::DisplayObjectPtr create(object2d::DisplayObjectType type);
        object3d::RenderObjectPtr  create(object3d::RenderObjectType type);
        
        float getCoordSystemWidth() const;
        float getCoordSystemHeight() const;
        float getDPIFactorX() const;
        float getDPIFactorY() const;
        
        math::p2d getCoordSystemDimension() const;
        math::p2d getDPIFactor() const;

        template <typename F, typename ...ARGS> void postponedDispatch(F func, ARGS... args);

        render::CameraInterface &getCamera();

        object2d::DisplayObjectInterface *&root2D;
        object3d::RenderObjectInterface  *&root3D;

        GameAPI(Engine &eng);
        virtual ~GameAPI();

    protected:
        Engine &_engine;
    };

    template <typename F, typename ...ARGS> void GameAPI::postponedDispatch(F func, ARGS... args) {
        _engine.postponedDispatch(func, args...);
    }
}