
//TODO: getting screen width/height

namespace fg {
    class Engine;

    struct GameAPI {        
        resources::ResourceManagerInterface  &resources;
        input::InputManagerInterface         &input;
        
        object2d::DisplayObjectPtr create(object2d::DisplayObjectType type);
        object3d::RenderObjectPtr  create(object3d::RenderObjectType type);

        float getScreenWidth() const;
        float getScreenHeight() const;

        render::CameraInterface &getCamera();

        object2d::DisplayObjectInterface *&root2D;
        object3d::RenderObjectInterface  *&root3D;

        GameAPI(Engine &eng);
        virtual ~GameAPI();

    protected:
        Engine &_engine;
    };
}