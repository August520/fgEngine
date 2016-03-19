
namespace fg {    
    namespace render {
        struct DefaultFrameConstants {
            float       screenWidth;
            float       screenHeight;
            float       environmentIntensity;
            float       environmentMipCount;
        };

        struct DefaultCameraConstants {
            math::m4x4  camViewProj;
            math::p4d   camPosition;
            math::p4d   pointOfInterest;
        };

        struct DefaultMaterialConstants {
            math::p3d   metalness;
            float       glossiness;
        };

        struct DefaultLightingConstants {
            math::p4d   lightPosAndDistances[FG_DEFAULT_LIGHTS_MAX];
            fg::color   lightColors[FG_DEFAULT_LIGHTS_MAX];
            unsigned    lightCount;
            float       shadowSpreadFactor;
            float       received0;
            float       received1;
        };

        //---

        class CameraInterface {
        public:
            virtual ~CameraInterface() {}

            virtual void  set(const CameraInterface &cam) = 0;
            virtual void  setLookAtByUp(const math::p3d &pos, const math::p3d &target, const math::p3d &upVector) = 0;
            virtual void  setLookAtByRight(const math::p3d &pos, const math::p3d &target, const math::p3d &rightVector) = 0;
            virtual void  setOrientation(const math::quat &q) = 0;
            virtual void  setTransform(const math::m4x4 &transform) = 0;
            virtual void  setPerspectiveProj(float fovY, float zNear, float zFar) = 0;
            virtual void  setInterestOffset(float offset) = 0;

            virtual const math::p3d &getPosition() const = 0;
            virtual const math::p3d &getTarget() const = 0;
            virtual const math::p3d &getInterestPoint() const = 0;

            virtual const math::p3d &getForwardDir() const = 0;
            virtual const math::p3d &getRightDir() const = 0;
            virtual const math::p3d &getUpDir() const = 0;

            virtual float getZNear() const = 0;
            virtual float getZFar() const = 0;

            virtual math::m4x4  getVPMatrix() const = 0;
            virtual math::p3d   screenToWorld(const math::p2d &screenCoord) const = 0;
            virtual math::p2d   worldToScreen(const math::p3d &pointInWorld) const = 0;

            virtual void updateMatrix() = 0;

        protected: // optimization
            template <bool(*op)(float, float)> friend class ::fg::object3d::RenderObjectComponentCmp;  
            math::p3d  _position;                                   
        };

        //---

        class SceneCompositionInterface {
        public:
            virtual ~SceneCompositionInterface() {}

            virtual OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> &getRegularMeshEnumerator() = 0;
            virtual OrderedCollection <object3d::Model3DInterface::MeshComponentInterface> &getTransparentMeshEnumerator() = 0;
            virtual OrderedCollection <object3d::Particles3DInterface::EmitterComponentInterface> &getParticleEmitterEnumerator() = 0;
            virtual OrderedCollection <object3d::PointLightInterface> &getPointLightEnumerator() = 0;
            virtual OrderedCollection <object2d::Sprite2DInterface> &getSprite2DEnumerator() = 0;
            virtual OrderedCollection <object2d::TextFieldInterface> &getTextFieldEnumerator() = 0;
        };

        class EngineSceneCompositionInterface : public SceneCompositionInterface {
        public:
            virtual ~EngineSceneCompositionInterface() {}

            virtual void addRenderObject(object3d::RenderObjectInterface *ptr) = 0;
            virtual void addDisplayObject(object2d::DisplayObjectInterface *ptr) = 0;
            virtual void removeRenderObject(object3d::RenderObjectInterface *ptr) = 0;
            virtual void removeDisplayObject(object2d::DisplayObjectInterface *ptr) = 0;

            virtual void update(float frameTimeMs) = 0;
        };

        //---

        class RenderSupportInterface {
        public:
            virtual ~RenderSupportInterface() {}
            virtual CameraInterface &getCamera() = 0;

            virtual platform::InstanceDataInterface      *getDefaultInstanceData() = 0;
            virtual platform::RasterizerParamsInterface  *getDefaultRasterizerParams() = 0;
            virtual platform::BlenderParamsInterface     *getDefaultLerpBlenderParams() = 0;
            virtual platform::BlenderParamsInterface     *getDefaultAddBlenderParams() = 0;
            virtual platform::DepthParamsInterface       *getDefaultDepthParams() = 0;
            virtual platform::SamplerInterface           *getDefaultPointSampler() = 0;
            virtual platform::SamplerInterface           *getDefaultLinearSampler() = 0;
            
            virtual DefaultFrameConstants     &defFrameConst() = 0;
            virtual DefaultCameraConstants    &defCameraConst() = 0;
            virtual DefaultMaterialConstants  &defMaterialConst() = 0;
            virtual DefaultLightingConstants  &defLightingConst() = 0;
            virtual InstanceDataDefault       &defInstanceData() = 0;
            
            virtual void defFrameConstApplyChanges() = 0;
            virtual void defCameraConstApplyChanges() = 0;
            virtual void defMaterialConstApplyChanges() = 0;
            virtual void defLightingConstApplyChanges() = 0;
            virtual void defInstanceDataApplyChanges() = 0;

            virtual void setShader(const resources::ShaderResourceInterface *shader) = 0;
            virtual void setTexture(platform::TextureSlot slot, const resources::Texture2DResourceInterface *texture) = 0;
            virtual void setScissorRect(const math::p2d &center, const math::p2d &lt, const math::p2d &rb, bool resolutionDepended = false) = 0;
            virtual void setMaterialParams(const math::p3d &metalness, float glossiness, const platform::TextureCubeInterface *irr, const platform::TextureCubeInterface *env) = 0;
            
            virtual void drawQuad2D(const math::m3x3 &trfm, const resources::ClipData *clip, unsigned frame, const fg::color &c, float z, bool resolutionDepended = false) = 0;
            virtual void drawText2D(const std::string &utf8text, const math::m3x3 &trfm, const resources::FontResourceInterface *font, const object2d::FontForm &form = object2d::FontForm(), object2d::TextAlign align = object2d::TextAlign::LEFT, bool resolutionDepended = false) = 0;
            virtual void drawScreenQuad(float x, float y, float width, float height) = 0;
            virtual void drawMesh(const resources::MeshInterface *mesh, const platform::InstanceDataInterface *instanceData = nullptr) = 0; //TODO: instances
            
            virtual void debugDrawBox(const math::m4x4 &transform, const fg::color &c) = 0;
            virtual void debugDrawBox(const math::p3d &pMin, const math::p3d &pMax, const fg::color &c) = 0;
            virtual void debugDrawFillBox(const math::m4x4 &transform, const fg::color &c) = 0;
            virtual void debugDrawTriangle(const math::p3d &p1, const math::p3d &p2, const math::p3d &p3, const fg::color &c) = 0;
            virtual void debugDrawLine(const math::p3d &p1, const math::p3d &p2, const fg::color &c) = 0;
            virtual void debugDrawAxis() = 0;
        };

        class EngineRenderSupportInterface : public RenderSupportInterface {
        public:
            virtual void init(platform::EnginePlatformInterface &iplatform, resources::ResourceManagerInterface &iresMan) = 0;
            virtual void frameInit3D(float frameTimeMs) = 0;
            virtual void frameInit2D(float frameTimeMs, float scaleX, float scaleY, float dpiFactor) = 0;
            virtual void destroy() = 0;
        };

        //---
        
        struct RenderAPI {
            resources::ResourceManagerInterface  &resources;
            platform::PlatformInterface          &platform;
            render::RenderSupportInterface       &rendering;
            render::CameraInterface              &gameCamera;

            RenderAPI(platform::PlatformInterface &plfm, resources::ResourceManagerInterface &rman, RenderSupportInterface &rapi, CameraInterface &gcam) : platform(plfm), resources(rman), rendering(rapi), gameCamera(gcam) {}
        };

        class RenderInterface {
        public:
            virtual ~RenderInterface() {}

            virtual void init(RenderAPI &api) = 0;
            virtual void destroy() = 0;
            virtual void update(float frameTimeMs, RenderAPI &api) = 0;
            virtual void draw3D(SceneCompositionInterface &sceneComposition, RenderAPI &api) = 0;
            virtual void draw2D(SceneCompositionInterface &sceneComposition, RenderAPI &api) = 0;

            virtual const char *getRenderResourceList() const = 0;
        };
    }
}


//