
namespace fg {
    namespace render {
        DefaultRender::DefaultRender() {

        }

        DefaultRender::~DefaultRender() {

        }

        void DefaultRender::init(RenderAPI &api) {
            
        }

        void DefaultRender::destroy() {

        }

        void DefaultRender::update(float frameTimeMs, RenderAPI &api) {

        }

        void DefaultRender::draw3D(object3d::RenderObjectIteratorInterface &iterator, RenderAPI &api) {
            api.platform.rdSetRenderTarget(api.platform.rdGetDefaultRenderTarget());
            api.platform.rdClearCurrentDepthBuffer();
            api.platform.rdClearCurrentColorBuffer();
            api.rendering.debugDrawAxis();

            while(iterator.next()) {

            }
        }

        void DefaultRender::draw2D(object2d::DisplayObjectIteratorInterface &iterator, RenderAPI &api) {
            while(iterator.next()) {
                if(iterator.type() == object2d::DisplayObjectType::SPRITE) {
                    
                }
            }
        }

        const char *DefaultRender::getRenderResourceList() const {
            return "";
        }
    }
}