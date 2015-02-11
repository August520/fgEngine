
namespace fg {
    namespace render {
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
            
        };
    }
}



