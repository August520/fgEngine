
namespace fg {
    namespace render {
        class RenderSupport : public EngineRenderSupportInterface {
        public:
            RenderSupport();
            ~RenderSupport() override;
            
            void init(platform::EnginePlatformInterface &iplatform, resources::ResourceManagerInterface &iresMan) override;
            void frameInit3D(float frameTimeMs) override;
            void frameInit2D(float frameTimeMs, float scaleX, float scaleY, float dpiFactor) override;
            void destroy() override;

            CameraInterface &getCamera() override;
            
            platform::InstanceDataInterface      *getDefaultInstanceData() override;
            platform::RasterizerParamsInterface  *getDefaultRasterizerParams() override;
            platform::BlenderParamsInterface     *getDefaultLerpBlenderParams() override;
            platform::BlenderParamsInterface     *getDefaultAddBlenderParams() override;
            platform::DepthParamsInterface       *getDefaultDepthParams() override;
            platform::SamplerInterface           *getDefaultPointSampler() override;
            platform::SamplerInterface           *getDefaultLinearSampler() override;

            DefaultFrameConstants     &defFrameConst() override;
            DefaultMaterialConstants  &defMaterialConst() override;
            InstanceDataDefault       &defInstanceData() override;
            
            void defFrameConstApplyChanges() override;
            void defMaterialConstApplyChanges() override;
            void defInstanceDataApplyChanges() override;

            void setShader(const resources::ShaderResourceInterface *shader) override;
            void setTexture(platform::TextureSlot slot, const resources::Texture2DResourceInterface *texture) override;
            void setScissorRect(const math::p2d &center, const math::p2d &lt, const math::p2d &rb, bool resolutionDepended) override;
            void setMaterialParams(const math::p3d &metalness, float glossiness, const platform::TextureCubeInterface *irr, const platform::TextureCubeInterface *env) override;

            void drawQuad2D(const math::m3x3 &trfm, const resources::ClipData *clip, unsigned frame, const fg::color &c, bool resolutionDepended) override;
            void drawText2D(const std::string &utf8text, const math::m3x3 &trfm, const resources::FontResourceInterface *font, const object2d::FontForm &form, object2d::TextAlign align, bool resolutionDepended) override;
            void drawScreenQuad(float x, float y, float width, float height) override; 
            void drawMesh(const resources::MeshInterface *mesh, const platform::InstanceDataInterface *instanceData) override;

            void debugDrawBox(const math::m4x4 &transform, const fg::color &c) override;
            void debugDrawBox(const math::p3d &pMin, const math::p3d &pMax, const fg::color &c) override;
            void debugDrawFillBox(const math::m4x4 &transform, const fg::color &c) override;
            void debugDrawTriangle(const math::p3d &p1, const math::p3d &p2, const math::p3d &p3, const fg::color &c) override;
            void debugDrawLine(const math::p3d &p1, const math::p3d &p2, const fg::color &c) override;
            void debugDrawAxis() override;

        protected:
            CameraInterface *_camera = nullptr;

            platform::EnginePlatformInterface    *_platform = nullptr;
            platform::RasterizerParamsInterface  *_defRasterizerParams = nullptr;
            platform::BlenderParamsInterface     *_defLerpBlenderParams = nullptr;
            platform::BlenderParamsInterface     *_defAddBlenderParams = nullptr;
            platform::DepthParamsInterface       *_defDepthParams = nullptr;
            platform::SamplerInterface           *_defLinearSampler = nullptr;
            platform::SamplerInterface           *_defPointSampler = nullptr;

            platform::IndexedVertexBufferInterface  *_oddVertexBufferSimple = nullptr;
            platform::IndexedVertexBufferInterface  *_oddVertexBufferTextured = nullptr;
            platform::IndexedVertexBufferInterface  *_oddVertexBufferNormal = nullptr;

            InstanceDataDefault                  _defInstanceStruct;
            InstanceDataDisplayObject            _defDisplayObjectInstanceStruct;
            platform::InstanceDataInterface      *_defInstanceData = nullptr;
            platform::InstanceDataInterface      *_defDisplayObjectInstanceData = nullptr;

            ShaderConstantBufferStruct           <DefaultFrameConstants> *_frameConstants = nullptr;
            ShaderConstantBufferStruct           <DefaultMaterialConstants>  *_materialConstants = nullptr;
            
            const resources::ShaderResourceInterface *_simpleShader = nullptr;
            const resources::ShaderResourceInterface *_ifaceShader = nullptr;

            float  _screenPixelsPerCoordSystemPixelsX;
            float  _screenPixelsPerCoordSystemPixelsY;
            float  _systemDpiPerCoordSystemDpi;

        private:
            RenderSupport(const RenderSupport &);
            RenderSupport &operator =(const RenderSupport &);
        };
    }
}


//
