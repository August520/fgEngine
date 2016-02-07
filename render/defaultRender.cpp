
namespace fg {
    namespace render {
        DefaultRender::DefaultRender() {

        }

        DefaultRender::~DefaultRender() {

        }

        void DefaultRender::init(RenderAPI &api) {
            _irradiance = ((resources::TextureCubeResourceInterface *)api.resources.getResource("defaultIrradiance.cubemap"))->getPlatformObject();
            _environments[0] = ((resources::TextureCubeResourceInterface *)api.resources.getResource("defaultEnvironment0.cubemap"))->getPlatformObject();
            _environments[1] = ((resources::TextureCubeResourceInterface *)api.resources.getResource("defaultEnvironment1.cubemap"))->getPlatformObject();
            _environments[2] = ((resources::TextureCubeResourceInterface *)api.resources.getResource("defaultEnvironment2.cubemap"))->getPlatformObject();
            _environments[3] = ((resources::TextureCubeResourceInterface *)api.resources.getResource("defaultEnvironment3.cubemap"))->getPlatformObject();
            _environments[4] = ((resources::TextureCubeResourceInterface *)api.resources.getResource("defaultEnvironment4.cubemap"))->getPlatformObject();
            _environments[5] = ((resources::TextureCubeResourceInterface *)api.resources.getResource("defaultEnvironment5.cubemap"))->getPlatformObject();
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

        void DefaultRender::draw3D(SceneCompositionInterface &sceneComposition, RenderAPI &api) {
            api.platform.rdSetRenderTarget(api.platform.rdGetDefaultRenderTarget());
            api.platform.rdClearCurrentDepthBuffer();
            api.platform.rdClearCurrentColorBuffer(fg::color(0.2f, 0.2f, 0.2f, 1.0f));
            api.rendering.debugDrawAxis();

            //--- LIGHTS ---

            auto &pointLights = sceneComposition.getPointLightEnumerator();
            api.rendering.defFrameConst().lightsCount = std::min(FG_DEFAULT_LIGHTS_MAX, pointLights.count());

            for (unsigned i = 0; i < api.rendering.defFrameConst().lightsCount && pointLights.next(); i++) {
                math::p3d pos  = pointLights.get()->getPosition();
                float distance = pointLights.get()->getDistance();
                
                api.rendering.defFrameConst().lightPosAndDistances[i] = math::p4d(pos.x, pos.y, pos.z, distance);
                api.rendering.defFrameConst().lightColors[i] = pointLights.get()->getColor();
            }

            api.rendering.defFrameConstApplyChanges();

            //--- REGULAR MESHES ---

            auto &regularMeshes = sceneComposition.getRegularMeshEnumerator();

            while (regularMeshes.next()) {
                const object3d::Model3DInterface::MeshComponentInterface *component = regularMeshes.get();

                if (component->isVisible()) {
                    for (unsigned i = 0; i < component->getTextureBindCount(); i++) {
                        api.rendering.setTexture(platform::TextureSlot(i), component->getTextureBind(i));
                    }

                    if (component->isSkinned()) {
                        platform::ShaderConstantBufferInterface *skinTable = component->getMesh()->getSkinConstBuffer();
                        skinTable->update(component->getSkinMatrixArray());
                        api.platform.rdSetShaderConstBuffer(skinTable);
                    }

                    unsigned envIndex = std::min(unsigned(component->getMaterialGlossiness() * float(FG_DEFAULT_ENV_MIPS)), FG_DEFAULT_ENV_MIPS - 1);
                    api.rendering.setMaterialParams(component->getMaterialMetalness(), component->getMaterialGlossiness(), _irradiance, _environments[envIndex]);
                    api.rendering.defInstanceData().rgba = fg::color();
                    api.rendering.defInstanceData().modelTransform = component->getFullTransform();
                    api.rendering.defInstanceDataApplyChanges();
                    api.rendering.setShader(component->getShader());
                    api.rendering.drawMesh(component->getMesh());
                }

            }

        }

        void DefaultRender::draw2D(SceneCompositionInterface &sceneComposition, RenderAPI &api) {
            auto &sprites = sceneComposition.getSprite2DEnumerator();

            while (sprites.next()) {
                const object2d::Sprite2DInterface *sprite = sprites.get();

                const fg::color rgba(1, 1, 1, sprite->getAlpha());
                const math::p2d center(sprite->getFullTransform()._31, sprite->getFullTransform()._32);

                api.rendering.setTexture(platform::TextureSlot::TEXTURE0, sprite->getTexture());
                api.rendering.setScissorRect(center, sprite->getScissorRectLT(), sprite->getScissorRectRB(), sprite->isResolutionDepended());
                api.rendering.drawQuad2D(sprite->getFullTransform(), sprite->getClipData(), sprite->getCurrentFrame(), rgba, sprite->getZ(), sprite->isResolutionDepended());
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