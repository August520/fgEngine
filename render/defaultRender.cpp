
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
            api.platform.rdClearCurrentColorBuffer(fg::color(0.5f, 0.5f, 0.5f, 1.0f));
            api.rendering.debugDrawAxis();

            while(iterator.next()) {
                if (iterator.type() == object3d::RenderObjectType::MODEL) {
                    const object3d::Model3DInterface *mdl = iterator.object();
                    const object3d::Model3DInterface::MeshComponentInterface *component = iterator.component();

                    if (component->isVisible()) {
                        for (unsigned i = 0; i < component->getTextureBindCount(); i++) {
                            api.rendering.setTexture(platform::TextureSlot(i), component->getTextureBind(i));
                        }

                        if (component->isSkinned()) {
                            platform::ShaderConstantBufferInterface *skinTable = component->getMesh()->getSkinConstBuffer();
                            skinTable->update(component->getSkinMatrixArray());
                            api.platform.rdSetShaderConstBuffer(skinTable);
                        }

                        api.rendering.defInstanceData().rgba = mdl->getColor(); 
                        api.rendering.defInstanceData().modelTransform = component->getFullTransform();
                        api.rendering.defInstanceDataApplyChanges();
                        api.rendering.setShader(component->getShader());
                        api.rendering.drawMesh(component->getMesh());
                    }
                }
                else if (iterator.type() == object3d::RenderObjectType::PARTICLES) {
                    const fg::object3d::Particles3DInterface *ptc = iterator.object();
                    const fg::object3d::Particles3DInterface::EmitterComponentInterface *component = iterator.component();

                    for (unsigned i = 0; i < component->getTextureBindCount(); i++) {
                        api.rendering.setTexture(fg::platform::TextureSlot(i), component->getTextureBind(i));
                    }

                    api.rendering.setShader(component->getShader());
                    _transparentDrawer.drawParticles(api, component);
                }
            }
        }

        void DefaultRender::draw2D(object2d::DisplayObjectIteratorInterface &iterator, RenderAPI &api) {
            while(iterator.next()) {
                if(iterator.type() == object2d::DisplayObjectType::SPRITE) {
                    const object2d::Sprite2DInterface *sprite = iterator.object();
                    const fg::color rgba(1, 1, 1, sprite->getAlpha());
                    const math::p2d center(sprite->getFullTransform()._31, sprite->getFullTransform()._32);

                    api.rendering.setTexture(platform::TextureSlot::TEXTURE0, sprite->getTexture());
                    api.rendering.setScissorRect(center, sprite->getScissorRectLT(), sprite->getScissorRectRB(), sprite->isResolutionDepended());
                    api.rendering.drawQuad2D(sprite->getFullTransform(), sprite->getClipData(), sprite->getCurrentFrame(), rgba, sprite->isResolutionDepended());
                }
                else if (iterator.type() == object2d::DisplayObjectType::TEXTFIELD) {
                    const fg::object2d::TextFieldInterface *obj = iterator.object();
                    api.rendering.drawText2D(obj->getText(), obj->getFullTransform(), obj->getFont(), obj->getForm(), obj->getAlign(), obj->isResolutionDepended());
                }
            }
            
            char buffer[64];
            sprintf(buffer, "FPS = %2.0f", _fps); //
            api.rendering.drawText2D(buffer, fg::math::m3x3(), api.resources.getResource("arial.ttf"), object2d::FontForm());
        }

        const char *DefaultRender::getRenderResourceList() const {
            return "";
        }
    }
}