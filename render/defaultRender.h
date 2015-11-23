
namespace fg {
    namespace render {
        // Just renders primitives. There's no optimization, reordering or other.
        //
        class DefaultRender : public RenderInterface {
        public:
            DefaultRender();
            ~DefaultRender() override;

            void init(RenderAPI &api) override;
            void destroy() override;
            void update(float frameTimeMs, RenderAPI &api) override;
            void draw3D(object3d::RenderObjectIteratorInterface &iterator, RenderAPI &api) override;
            void draw2D(object2d::DisplayObjectIteratorInterface &iterator, RenderAPI &api) override;

            const char *getRenderResourceList() const override;

        protected:
            fg::render::TransparentDrawer <1024> _transparentDrawer;

            float _frameCount = 0.0f;
            float _countedTime = 0.0f;
            float _fps = 0.0f;            
        };
    }
}



