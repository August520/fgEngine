
namespace fg {
    namespace render {
        struct AdditionalData {
            math::p4d lightPositionAndDistance;
        };

        class DefaultRender : public RenderInterface {
        public:
            DefaultRender();
            ~DefaultRender() override;

            void init(RenderAPI &api) override;
            void destroy() override;
            void update(float frameTimeMs, RenderAPI &api) override;
            void draw3D(SceneCompositionInterface &sceneComposition, RenderAPI &api) override;
            void draw2D(SceneCompositionInterface &sceneComposition, RenderAPI &api) override;

            const char *getRenderResourceList() const override;

        protected:
            const platform::TextureCubeInterface *_irradiance = nullptr;
            const platform::TextureCubeInterface *_environments[FG_DEFAULT_ENV_MIPS] = {nullptr};

            platform::RasterizerParamsInterface  *_shadowRasterizer = nullptr;
            platform::RasterizerParamsInterface  *_twoSideRasterizer = nullptr;
            platform::RasterizerParamsInterface  *_currentRasterizer = nullptr;
            platform::BlenderParamsInterface     *_shadowBlend = nullptr;
            platform::CubeRenderTargetInterface  *_shadowCubeRT[FG_DEFAULT_LIGHTS_MAX] = {nullptr};
            ShaderConstantBufferStruct           <AdditionalData> *_additionalConstants;

            float _frameCount = 0.0f;
            float _countedTime = 0.0f;
            float _fps = 0.0f;            
        };
    }
}



