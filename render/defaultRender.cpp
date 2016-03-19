
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

            _shadowRasterizer = api.platform.rdCreateRasterizerParams(platform::CullMode::BACK);
            _shadowBlend = api.platform.rdCreateBlenderParams(platform::BlendMode::MIN_VALUE);
            _additionalConstants = new ShaderConstantBufferStruct <AdditionalData> (api.platform, platform::ShaderConstBufferUsing::ADDITIONAL_DATA);

            for (unsigned i = 0; i < FG_DEFAULT_LIGHTS_MAX; i++) {
                _shadowCubeRT[i] = api.platform.rdCreateCubeRenderTarget(256, platform::RenderTargetType::OnlyColorNullDepth);
            }
        }

        void DefaultRender::destroy() {
            if (_shadowBlend) {
                _shadowBlend->release();
                _shadowBlend = nullptr;
            }
            if (_shadowRasterizer) {
                _shadowRasterizer->release();
                _shadowRasterizer = nullptr;
            }
            if (_additionalConstants) {
                delete _additionalConstants;
                _additionalConstants = nullptr;
            }
        
            for (unsigned i = 0; i < FG_DEFAULT_LIGHTS_MAX; i++) {
                if (_shadowCubeRT[i]) {
                    _shadowCubeRT[i]->release();
                    _shadowCubeRT[i] = nullptr;
                }
            }
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
            auto &regularMeshes = sceneComposition.getRegularMeshEnumerator();

            //--- LIGHTS & SHADOWS ---

            auto &pointLights = sceneComposition.getPointLightEnumerator();
            api.rendering.defLightingConst().lightCount = std::min(FG_DEFAULT_LIGHTS_MAX, pointLights.count());
            api.rendering.defLightingConst().shadowSpreadFactor = 0.8f;

            api.platform.rdSetSampler(platform::TextureSlot::TEXTURE0, api.rendering.getDefaultPointSampler());
            api.platform.rdSetRasterizerParams(_shadowRasterizer);
            api.platform.rdSetBlenderParams(_shadowBlend);
            api.rendering.setShader(api.resources.getResource("depthR32FModel.shader"));

            static const math::p3d cubeFdDirs[] = {
                math::p3d(-1, 0, 0), math::p3d(1, 0, 0),
                math::p3d(0, -1, 0), math::p3d(0, 1, 0),
                math::p3d(0, 0, -1), math::p3d(0, 0, 1),
            };
            static const math::p3d cubeUpDirs[] = {
                math::p3d(0, 1, 0), math::p3d(0, 1, 0),
                math::p3d(0, 0, -1), math::p3d(0, 0, 1),
                math::p3d(0, 1, 0), math::p3d(0, 1, 0)};

            for (unsigned i = 0; i < api.rendering.defLightingConst().lightCount && pointLights.next(); i++) { //
                math::p3d pos = pointLights.get(i)->getPosition();
                float distance = pointLights.get(i)->getDistance();

                api.rendering.defLightingConst().lightPosAndDistances[i] = math::p4d(pos.x, pos.y, pos.z, distance);
                api.rendering.defLightingConst().lightColors[i] = pointLights.get(i)->getColor();

                _additionalConstants->data.lightPositionAndDistance = math::p4d(pos.x, pos.y, pos.z, distance);
                _additionalConstants->updateAndApply();

                for (unsigned c = 0; c < 6; c++) {
                    regularMeshes.resetIteration();

                    api.platform.rdSetCubeRenderTarget(_shadowCubeRT[i], c);
                    api.platform.rdClearCurrentColorBuffer(color(1.0f, 1.0f, 1.0f, 1.0f));
                    
                    math::m4x4 cubeSideProj, cubeSideView;
                    cubeSideProj.perspectiveFovLH(0.5f * M_PI, 1.0f, 0.1f, 20.0f);
                    cubeSideView.lookAt(pos, pos + cubeFdDirs[c], cubeUpDirs[c]);

                    api.rendering.defCameraConst().camViewProj = cubeSideView * cubeSideProj;
                    api.rendering.defCameraConstApplyChanges();

                    while (regularMeshes.next()) {
                        const object3d::Model3DInterface::MeshComponentInterface *component = regularMeshes.get();

                        if (component->isVisible()) {
                            api.rendering.defInstanceData().rgba = color();
                            api.rendering.defInstanceData().modelTransform = component->getFullTransform();
                            api.rendering.defInstanceDataApplyChanges();
                            api.rendering.drawMesh(component->getMesh());
                        }
                    }
                }
            }
            
            api.rendering.defLightingConstApplyChanges();
            api.platform.rdSetRenderTarget(api.platform.rdGetDefaultRenderTarget());
            
            //--- REGULAR MESHES ---

            api.platform.rdClearCurrentDepthBuffer();
            api.platform.rdClearCurrentColorBuffer();

            for (unsigned i = 0; i < api.rendering.defLightingConst().lightCount; i++) {
                api.platform.rdSetTextureCube(platform::TextureSlot(FG_SHADOW_TEXTURES_BASE + i), _shadowCubeRT[i]->getRenderBuffer());
            }

            api.platform.rdSetBlenderParams(api.rendering.getDefaultLerpBlenderParams());
            api.platform.rdSetRasterizerParams(api.rendering.getDefaultRasterizerParams());
            api.platform.rdSetSampler(platform::TextureSlot::TEXTURE0, api.rendering.getDefaultLinearSampler());
            
            api.rendering.getCamera().set(api.gameCamera);
            api.rendering.defCameraConst().camViewProj = api.rendering.getCamera().getVPMatrix();
            api.rendering.defCameraConstApplyChanges();
                      
            //api.rendering.debugDrawAxis();

            regularMeshes.resetIteration();
            
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
                    api.rendering.defInstanceData().rgba = color();
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