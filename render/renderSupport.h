
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
            platform::BlenderParamsInterface     *getDefaultBlenderParams() override;
            platform::DepthParamsInterface       *getDefaultDepthParams() override;
            platform::SamplerInterface           *getDefaultPointSampler() override;
            platform::SamplerInterface           *getDefaultLinearSampler() override;

            DefaultFrameConstants  &defFrameConst() override;
            InstanceDataDefault    &defInstanceData() override;
            
            void defFrameConstApplyChanges() override;
            void defInstanceDataApplyChanges() override;

            void setShader(const resources::ShaderResourceInterface *shader) override;
            void setTexture(platform::TextureSlot slot, const resources::Texture2DResourceInterface *texture) override;

            void drawQuad2D(const math::m3x3 &trfm, const resources::ClipData *clip, unsigned frame, const fg::color &c, bool resolutionDepended) override;
            void drawText2D(const fg::string &utf8text, const math::m3x3 &trfm, const resources::FontResourceInterface *font, unsigned size, const fg::color &c, bool resolutionDepended) override;
            void drawScreenQuad(float x, float y, float width, float height) override; 
            void drawMesh(const resources::MeshInterface *mesh, const platform::InstanceDataInterface *instanceData) override;

            void debugDrawBox(const math::m4x4 &transform, const fg::color &c) override;
            void debugDrawBox(const math::p3d &pMin, const math::p3d &pMax, const fg::color &c) override;
            void debugDrawFillBox(const math::m4x4 &transform, const fg::color &c) override;
            void debugDrawTriangle(const math::p3d &p1, const math::p3d &p2, const math::p3d &p3, const fg::color &c) override;
            void debugDrawLine(const math::p3d &p1, const math::p3d &p2, const fg::color &c) override;
            void debugDrawAxis() override;

        protected:
            CameraInterface *_camera;

            platform::EnginePlatformInterface    *_platform;
            platform::RasterizerParamsInterface  *_defRasterizerParams;
            platform::BlenderParamsInterface     *_defBlenderParams;
            platform::DepthParamsInterface       *_defDepthParams;
            platform::SamplerInterface           *_defLinearSampler;
            platform::SamplerInterface           *_defPointSampler;

            platform::IndexedVertexBufferInterface  *_oddVertexBufferSimple;
            platform::IndexedVertexBufferInterface  *_oddVertexBufferTextured;
            platform::IndexedVertexBufferInterface  *_oddVertexBufferNormal;

            InstanceDataDefault                  _defInstanceStruct;
            InstanceDataDisplayObject            _defDisplayObjectInstanceStruct;
            platform::InstanceDataInterface      *_defInstanceData;
            platform::InstanceDataInterface      *_defDisplayObjectInstanceData;

            ShaderConstantBufferStruct           <DefaultFrameConstants> *_frameConstants;
            
            const resources::ShaderResourceInterface *_simpleShader;
            const resources::ShaderResourceInterface *_ifaceShader;

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
