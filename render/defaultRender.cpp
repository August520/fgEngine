
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
            _countedTime += frameTimeMs;
            _frameCount += 1.0f;

            if(_countedTime >= 1000.0f) {
                _fps = _frameCount;
                _countedTime = 0.0f;
                _frameCount = 0.0f;
            }
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
            
            char buffer[64];
            sprintf(buffer, "FPS = %2.4f", _fps); //
            api.rendering.drawText2D(buffer, fg::math::m3x3(), api.resources.getResource("arial.ttf"), 18, fg::color(0.8f, 0.9f, 1.0f, 1.0f));            
        }

        const char *DefaultRender::getRenderResourceList() const {
            return "";
        }
    }
}