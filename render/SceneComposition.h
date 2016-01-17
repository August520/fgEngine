
#include <unordered_map>

namespace fg {
    namespace render {
        class SceneComposition : public EngineSceneCompositionInterface {
        public:
            SceneComposition(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan, CameraInterface *cam);
            ~SceneComposition() override;

            OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> &getRegularMeshEnumerator() override;
            OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> &getTransparentMeshEnumerator() override;
            OrderedCollection <object3d::Particles3DInterface::EmitterComponentInterface> &getParticleEmitterEnumerator() override;
            OrderedCollection <object3d::PointLightInterface> &getPointLightEnumerator() override;
            OrderedCollection <object2d::Sprite2DInterface> &getSprite2DEnumerator() override;
            OrderedCollection <object2d::TextFieldInterface> &getTextFieldEnumerator() override;

            void addRenderObject(object3d::RenderObjectInterface *ptr) override;
            void addDisplayObject(object2d::DisplayObjectInterface *ptr) override;
            void removeRenderObject(object3d::RenderObjectInterface *ptr) override;
            void removeDisplayObject(object2d::DisplayObjectInterface *ptr) override;

            void update(float frameTimeMs) override;

        protected:            
            platform::PlatformInterface &_platform;
            resources::ResourceManagerInterface &_resMan;
            CameraInterface *_camera;

            std::unordered_map <object3d::RenderObjectInterface *, bool> _renderObjects;
            std::unordered_map <object2d::DisplayObjectInterface *, bool> _displayObjects;
            
            OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> _regularMeshes;
            OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> _transparentMeshes;
            OrderedCollection <object3d::Particles3DInterface::EmitterComponentInterface> _particleEmitters;
            OrderedCollection <object3d::PointLightInterface> _pointLights;
            OrderedCollection <object2d::Sprite2DInterface> _sprites;
            OrderedCollection <object2d::TextFieldInterface> _textFields;

            void _removeRenderObject(object3d::RenderObjectInterface *ptr);
            void _removeDisplayObject(object2d::DisplayObjectInterface *ptr);
        };
    }
}

