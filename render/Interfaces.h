
namespace fg {
    namespace render {
        // TODO: all DRAW constants -> instance data (+ texture index too)

        const unsigned FG_SKIN_MATRIX_MAX = 32;

        struct DefaultFrameConstants {
            math::m4x4  camViewProj;
            math::p3d   camPosition;
            float       scrWidth;
            math::p3d   globalSunDirection;
            float       scrHeight;
        };

        struct DefaultDrawConstants { 
            math::m4x4       modelTransform;
            platform::color  rgba;
        };
        
        class CameraInterface {
        public:
            virtual ~CameraInterface() {}

            virtual void  set(const CameraInterface &cam) = 0;
            virtual void  setLookAtByUp(const math::p3d &pos, const math::p3d &target, const math::p3d &upVector) = 0;
            virtual void  setLookAtByRight(const math::p3d &pos, const math::p3d &target, const math::p3d &rightVector) = 0;
            virtual void  setOrientation(const math::quat &q) = 0;
            virtual void  setTransform(const math::m4x4 &transform) = 0;
            virtual void  setPerspectiveProj(float fov, float zNear, float zFar) = 0;

            virtual const math::p3d &getPosition() const = 0;
            virtual const math::p3d &getTarget() const = 0;

            virtual math::m4x4  getVPMatrix() const = 0;
            virtual math::m4x4  getOrientedVPMatrix() const = 0;
            virtual math::p3d   screenToWorld(const math::p2d &screenCoord) const = 0;
            virtual math::p2d   worldToScreen(const math::p3d &pointInWorld) const = 0;
        };
        
        class RenderSupportInterface {
        public:
            virtual ~RenderSupportInterface() {}
            virtual CameraInterface &getCamera() = 0;

            virtual platform::RasterizerParamsInterface  *getDefaultRasterizerParams() = 0;
            virtual platform::BlenderParamsInterface     *getDefaultBlenderParams() = 0;
            virtual platform::DepthParamsInterface       *getDefaultDepthParams() = 0;
            virtual platform::SamplerInterface           *getDefaultPointSampler() = 0;
            virtual platform::SamplerInterface           *getDefaultLinearSampler() = 0;

            virtual DefaultFrameConstants  &defFrameConst() = 0;
            virtual DefaultDrawConstants   &defDrawConst() = 0;
            
            virtual void defFrameConstApplyChanges() = 0;
            virtual void defDrawConstApplyChanges() = 0;

            virtual void setShader(const resources::ShaderResourceInterface *shader) = 0;
            virtual void setTexture(platform::TextureSlot slot, const resources::Texture2DResourceInterface *texture) = 0;

            virtual void drawQuad2D(const math::m3x3 &trfm, float z, const resources::ClipData *clip, unsigned frame) = 0;
            virtual void drawText2D(const fg::string &utf8text, const math::m3x3 &trfm, float z, const resources::FontResourceInterface *font, unsigned size) = 0;
            virtual void drawScreenQuad(float x, float y, float width, float height) = 0;
            virtual void drawMesh(const resources::MeshInterface *mesh) = 0;

            virtual void debugDrawBox(const math::m4x4 &transform, const platform::color &c) = 0;
            virtual void debugDrawBox(const math::p3d &pMin, const math::p3d &pMax, const platform::color &c) = 0;
            virtual void debugDrawFillBox(const math::m4x4 &transform, const platform::color &c) = 0;
            virtual void debugDrawTriangle(const math::p3d &p1, const math::p3d &p2, const math::p3d &p3, const platform::color &c) = 0;
            virtual void debugDrawLine(const math::p3d &p1, const math::p3d &p2, const platform::color &c) = 0;
            virtual void debugDrawAxis() = 0;
        };

        class EngineRenderSupportInterface : public RenderSupportInterface {
        public:
            virtual void init(platform::EnginePlatformInterface &iplatform, resources::ResourceManagerInterface &iresMan) = 0;
            virtual void frameInit3D(float frameTimeMs) = 0;
            virtual void frameInit2D(float frameTimeMs) = 0;
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
            virtual void draw3D(object3d::RenderObjectIteratorInterface &iterator, RenderAPI &api) = 0;
            virtual void draw2D(object2d::DisplayObjectIteratorInterface &iterator, RenderAPI &api) = 0;

            virtual const char *getRenderResourceList() const = 0;
        };
    }
}


//