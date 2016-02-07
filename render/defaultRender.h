
namespace fg {
    namespace render {
        const unsigned FG_DEFAULT_ENV_MIPS = 6;

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
            const platform::TextureCubeInterface *_irradiance;
            const platform::TextureCubeInterface *_environments[FG_DEFAULT_ENV_MIPS];
            fg::render::TransparentDrawer <1024> _transparentDrawer;

            float _frameCount = 0.0f;
            float _countedTime = 0.0f;
            float _fps = 0.0f;            
        };
    }
}



