
// TODO: draw* over instances

namespace fg {
    namespace render {
        const unsigned ODDBUFFER_SIMPLE_MAX = 32;
        const unsigned ODDBUFFER_NORMAL_MAX = 32;
        const unsigned ODDBUFFER_TEXTURED_MAX = 512;

        RenderSupport::RenderSupport() {
            _camera = nullptr;
            _platform = nullptr;

            _defRasterizerParams = nullptr;
            _defBlenderParams = nullptr;
            _defDepthParams = nullptr;
            _defLinearSampler = nullptr;
            _defPointSampler = nullptr;

            _frameConstants = nullptr;
            _simpleShader = nullptr;
            _ifaceShader = nullptr;

            _defInstanceData = nullptr;
            _defDisplayObjectInstanceData = nullptr;
        }

        RenderSupport::~RenderSupport() {
        
        }

        void RenderSupport::init(platform::EnginePlatformInterface &iplatform, resources::ResourceManagerInterface &iresMan) {
            _platform = &iplatform;            
            _defRasterizerParams = iplatform.rdCreateRasterizerParams(platform::CullMode::NONE);
            _defBlenderParams = iplatform.rdCreateBlenderParams(platform::BlendMode::ALPHA_LERP);
            _defDepthParams = iplatform.rdCreateDepthParams(true, platform::DepthFunc::LESS_EQUAL, true);
            _defPointSampler = iplatform.rdCreateSampler(platform::TextureFilter::POINT, platform::TextureAddressMode::CLAMP);
            _defLinearSampler = iplatform.rdCreateSampler(platform::TextureFilter::LINEAR, platform::TextureAddressMode::CLAMP);
            _oddVertexBufferSimple = iplatform.rdCreateVertexBuffer(platform::VertexType::SIMPLE, ODDBUFFER_SIMPLE_MAX, true);
            _oddVertexBufferTextured = iplatform.rdCreateVertexBuffer(platform::VertexType::TEXTURED, ODDBUFFER_TEXTURED_MAX, true);
            _oddVertexBufferNormal = iplatform.rdCreateVertexBuffer(platform::VertexType::NORMAL, ODDBUFFER_NORMAL_MAX, true);
            _defInstanceData = iplatform.rdCreateInstanceData(platform::InstanceDataType::DEFAULT, 1);
            _defDisplayObjectInstanceData = iplatform.rdCreateInstanceData(platform::InstanceDataType::DISPLAY_OBJECT, 1);

            _camera = new Camera (iplatform);
            _frameConstants = new ShaderConstantBufferStruct <DefaultFrameConstants> (iplatform, platform::ShaderConstBufferUsing::FRAME_DATA);
            
            _simpleShader = iresMan.getResource(FG_SIMPLE_SHADER);
            _ifaceShader = iresMan.getResource(FG_IFACE_SHADER);
        }

        void RenderSupport::frameInit3D(float frameTimeMs) {
            _platform->rdSetSampler(platform::TextureSlot::TEXTURE0, _defLinearSampler);
            _platform->rdSetBlenderParams(_defBlenderParams);
            _platform->rdSetDepthParams(_defDepthParams);
            _platform->rdSetRasterizerParams(_defRasterizerParams);

            _frameConstants->data.camPosition = _camera->getPosition();
            _frameConstants->data.camViewProj = _camera->getVPMatrix();
            _frameConstants->data.globalSunDirection = math::p3d(1.0f, 1.0f, 1.0f).normalize();
            _frameConstants->data.scrWidth = _platform->getScreenWidth();
            _frameConstants->data.scrHeight = _platform->getScreenHeight();
            _frameConstants->updateAndApply();
            
            _platform->rdSetShader(_simpleShader->getPlatformObject());
        }

        void RenderSupport::frameInit2D(float frameTimeMs) {
            _frameConstants->data.camViewProj.identity();
            _frameConstants->updateAndApply();
            
            _platform->rdSetBlenderParams(_defBlenderParams);
            _platform->rdSetSampler(platform::TextureSlot::TEXTURE0, _defLinearSampler);
            _platform->rdSetShader(_ifaceShader->getPlatformObject());
        }

        void RenderSupport::destroy() {
            delete _camera;
            delete _frameConstants;
            
            _camera = nullptr;
            _frameConstants = nullptr;
            _platform = nullptr;
            _simpleShader = nullptr;
            _ifaceShader = nullptr;

            _defInstanceData->release();
            _defDisplayObjectInstanceData->release();
            _defRasterizerParams->release();
            _defBlenderParams->release();
            _defDepthParams->release();
            _defLinearSampler->release();
            _defPointSampler->release();
            _oddVertexBufferSimple->release();
            _oddVertexBufferTextured->release();
            _oddVertexBufferNormal->release();
            
            _defInstanceData = nullptr;
            _defDisplayObjectInstanceData = nullptr;
            _defRasterizerParams = nullptr;
            _defBlenderParams = nullptr;
            _defDepthParams = nullptr;
            _defLinearSampler = nullptr;
            _defPointSampler = nullptr;
            _oddVertexBufferSimple = nullptr;
            _oddVertexBufferTextured = nullptr;
            _oddVertexBufferNormal = nullptr;
        }

        CameraInterface &RenderSupport::getCamera() {
            return *_camera;
        }
        
        platform::InstanceDataInterface *RenderSupport::getDefaultInstanceData() {
            return _defInstanceData;
        }

        platform::RasterizerParamsInterface *RenderSupport::getDefaultRasterizerParams() {
            return _defRasterizerParams;            
        }

        platform::BlenderParamsInterface *RenderSupport::getDefaultBlenderParams() {
            return _defBlenderParams;            
        }

        platform::DepthParamsInterface *RenderSupport::getDefaultDepthParams() {
            return _defDepthParams;            
        }

        platform::SamplerInterface *RenderSupport::getDefaultPointSampler() {
            return _defPointSampler;            
        }

        platform::SamplerInterface *RenderSupport::getDefaultLinearSampler() {
            return _defLinearSampler;            
        }

        DefaultFrameConstants &RenderSupport::defFrameConst() {
            return _frameConstants->data;
        }
        
        InstanceDataDefault &RenderSupport::defInstanceData() {
            return _defInstanceStruct;
        }

        void RenderSupport::defFrameConstApplyChanges() {
            _frameConstants->updateAndApply();
        }

        void RenderSupport::defInstanceDataApplyChanges() {
            _defInstanceData->update(&_defInstanceStruct, 1);
        }

        void RenderSupport::setShader(const resources::ShaderResourceInterface *shader) {
            _platform->rdSetShader(shader->getPlatformObject());
        }

        void RenderSupport::setTexture(platform::TextureSlot slot, const resources::Texture2DResourceInterface *texture) {
            _platform->rdSetTexture2D(slot, texture->getPlatformObject());
        }
        
        void RenderSupport::drawQuad2D(const math::m3x3 &trfm, const resources::ClipData *clip, unsigned frame, const fg::color &c) {
            math::p2d lt (-clip->centerX, -clip->centerY);
            math::p2d lb (-clip->centerX, clip->height - clip->centerY);
            math::p2d rt (clip->width - clip->centerX, -clip->centerY);
            math::p2d rb (clip->width - clip->centerX, clip->height - clip->centerY);

            lt.transform(trfm, true);
            lb.transform(trfm, true);
            rt.transform(trfm, true);
            rb.transform(trfm, true);

            lt.x = 2.0f * lt.x / _platform->getScreenWidth() - 1.0f;
            lb.x = 2.0f * lb.x / _platform->getScreenWidth() - 1.0f;
            rt.x = 2.0f * rt.x / _platform->getScreenWidth() - 1.0f;
            rb.x = 2.0f * rb.x / _platform->getScreenWidth() - 1.0f;

            lt.y = 1.0f - 2.0f * lt.y / _platform->getScreenHeight();
            lb.y = 1.0f - 2.0f * lb.y / _platform->getScreenHeight();
            rt.y = 1.0f - 2.0f * rt.y / _platform->getScreenHeight();
            rb.y = 1.0f - 2.0f * rb.y / _platform->getScreenHeight();

            float sz = 0.0f;
            float tx = clip->frames[frame].tu;
            float ty = clip->frames[frame].tv;

            float txleft = tx / _platform->getTextureWidth(platform::TextureSlot::TEXTURE0);
            float txright = (tx + clip->width) / _platform->getTextureWidth(platform::TextureSlot::TEXTURE0);
            float tytop = ty / _platform->getTextureHeight(platform::TextureSlot::TEXTURE0);
            float tybottom = (ty + clip->height) / _platform->getTextureHeight(platform::TextureSlot::TEXTURE0);

            VertexTextured *tmem = (VertexTextured *)_oddVertexBufferTextured->lock();

            tmem[0].position.x = lb.x;
            tmem[0].position.y = lb.y;
            tmem[0].position.z = sz;
            tmem[0].uv.x = txleft;
            tmem[0].uv.y = tybottom;

            tmem[1].position.x = rb.x;
            tmem[1].position.y = rb.y;
            tmem[1].position.z = sz;
            tmem[1].uv.x = txright;
            tmem[1].uv.y = tybottom;

            tmem[2].position.x = lt.x;
            tmem[2].position.y = lt.y;
            tmem[2].position.z = sz;
            tmem[2].uv.x = txleft;
            tmem[2].uv.y = tytop;

            tmem[3].position.x = rt.x;
            tmem[3].position.y = rt.y;
            tmem[3].position.z = sz;
            tmem[3].uv.x = txright;
            tmem[3].uv.y = tytop;

            _oddVertexBufferTextured->unlock();
            
            _defDisplayObjectInstanceStruct.isGrey = 0.0f;
            _defDisplayObjectInstanceStruct.rgba = c;
            _defDisplayObjectInstanceData->update(&_defDisplayObjectInstanceStruct, 1);
            _platform->rdDrawGeometry(_oddVertexBufferTextured, _defDisplayObjectInstanceData, platform::PrimitiveTopology::TRIANGLE_STRIP, 4);
        }

        void RenderSupport::drawText2D(const fg::string &utf8text, const math::m3x3 &trfm, const resources::FontResourceInterface *font, unsigned size, const fg::color &c) {
            math::p2d rightDir (1.0f, 0.0f);
            math::p2d downDir (0.0f, 1.0f);

            rightDir.transform(trfm, false);
            downDir.transform(trfm, false);

            rightDir.x = 2.0f * rightDir.x / _platform->getScreenWidth();
            rightDir.y = -2.0f * rightDir.y / _platform->getScreenHeight();
            downDir.x = 2.0f * downDir.x / _platform->getScreenWidth();
            downDir.y = -2.0f * downDir.y / _platform->getScreenHeight();

            math::p2d lt (2.0f * int(trfm._31) / _platform->getScreenWidth() - 1.0f, 1.0f - 2.0f * int(trfm._32) / _platform->getScreenHeight());
            math::p2d ltOrigin = lt;

            unsigned i = 0;
            unsigned tchLen = 0;
            resources::FontCharInfo curCharData;

            platform::Texture2DInterface *curTexture = nullptr;
            VertexTextured *tmem = (VertexTextured *)_oddVertexBufferTextured->lock();

            for(const char *charPtr = utf8text.data(); charPtr[0] != 0; charPtr += tchLen, i++) {
                tchLen = fg::string::utf8CharLen(charPtr);
                font->getChar(charPtr, size, curCharData);

                if(*charPtr == '\n') {
                    lt = ltOrigin;
                    lt += downDir * curCharData.height;
                }
                else {
                    if(curTexture != curCharData.texture) {
                        curTexture = curCharData.texture;
                        _platform->rdSetTexture2D(platform::TextureSlot::TEXTURE0, curTexture);
                    }

                    lt += rightDir * floor(curCharData.lsb);
                    math::p2d lb = lt + downDir * float(size);
                    math::p2d rt = lt + rightDir * curCharData.width;
                    math::p2d rb = rt + downDir * float(size);

                    int index = i * 6;

                    tmem[index].position.x = lb.x;
                    tmem[index].position.y = lb.y;
                    tmem[index].position.z = 0;
                    tmem[index].uv.x = curCharData.tu;
                    tmem[index].uv.y = curCharData.tv + curCharData.txHeight;

                    index++;

                    tmem[index].position.x = rb.x;
                    tmem[index].position.y = rb.y;
                    tmem[index].position.z = 0;
                    tmem[index].uv.x = curCharData.tu + curCharData.txWidth;
                    tmem[index].uv.y = curCharData.tv + curCharData.txHeight;

                    index++;

                    tmem[index].position.x = lt.x;
                    tmem[index].position.y = lt.y;
                    tmem[index].position.z = 0;
                    tmem[index].uv.x = curCharData.tu;
                    tmem[index].uv.y = curCharData.tv;

                    index++;

                    tmem[index].position.x = lt.x;
                    tmem[index].position.y = lt.y;
                    tmem[index].position.z = 0;
                    tmem[index].uv.x = curCharData.tu;
                    tmem[index].uv.y = curCharData.tv;

                    index++;

                    tmem[index].position.x = rb.x;
                    tmem[index].position.y = rb.y;
                    tmem[index].position.z = 0;
                    tmem[index].uv.x = curCharData.tu + curCharData.txWidth;
                    tmem[index].uv.y = curCharData.tv + curCharData.txHeight;

                    index++;

                    tmem[index].position.x = rt.x;
                    tmem[index].position.y = rt.y;
                    tmem[index].position.z = 0;
                    tmem[index].uv.x = curCharData.tu + curCharData.txWidth;
                    tmem[index].uv.y = curCharData.tv;

                    lt += rightDir * floor(curCharData.advance - curCharData.lsb);
                }
            }

            _oddVertexBufferTextured->unlock();

            _defDisplayObjectInstanceStruct.isGrey = 1.0f;
            _defDisplayObjectInstanceStruct.rgba = c;
            _defDisplayObjectInstanceData->update(&_defDisplayObjectInstanceStruct, 1);
            _platform->rdDrawGeometry(_oddVertexBufferTextured, _defDisplayObjectInstanceData, platform::PrimitiveTopology::TRIANGLE_LIST, i * 6);
        }

        void RenderSupport::drawScreenQuad(float x, float y, float width, float height) {
            math::p2d lt (x, y);
            math::p2d lb (x, y + height);
            math::p2d rt (x + width, y);
            math::p2d rb (x + width, y + height);
            
            lt.x = 2.0f * lt.x / _platform->getScreenWidth() - 1.0f;
            lb.x = 2.0f * lb.x / _platform->getScreenWidth() - 1.0f;
            rt.x = 2.0f * rt.x / _platform->getScreenWidth() - 1.0f;
            rb.x = 2.0f * rb.x / _platform->getScreenWidth() - 1.0f;

            lt.y = 1.0f - 2.0f * lt.y / _platform->getScreenHeight();
            lb.y = 1.0f - 2.0f * lb.y / _platform->getScreenHeight();
            rt.y = 1.0f - 2.0f * rt.y / _platform->getScreenHeight();
            rb.y = 1.0f - 2.0f * rb.y / _platform->getScreenHeight();

            VertexTextured *tmem = (VertexTextured *)_oddVertexBufferTextured->lock();

            tmem[0].position.x = lb.x;
            tmem[0].position.y = lb.y;
            tmem[0].position.z = 0.0f;
            tmem[0].uv.x = 0.0f;
            tmem[0].uv.y = 1.0f;

            tmem[1].position.x = rb.x;
            tmem[1].position.y = rb.y;
            tmem[1].position.z = 0.0f;
            tmem[1].uv.x = 1.0f;
            tmem[1].uv.y = 1.0f;

            tmem[2].position.x = lt.x;
            tmem[2].position.y = lt.y;
            tmem[2].position.z = 0.0f;
            tmem[2].uv.x = 0.0f;
            tmem[2].uv.y = 0.0f;

            tmem[3].position.x = rt.x;
            tmem[3].position.y = rt.y;
            tmem[3].position.z = 0.0f;
            tmem[3].uv.x = 1.0f;
            tmem[3].uv.y = 0.0f;

            _oddVertexBufferTextured->unlock();
            _defDisplayObjectInstanceStruct.isGrey = 0.0f;
            _defDisplayObjectInstanceStruct.rgba = fg::color(1, 1, 1, 1);
            _defDisplayObjectInstanceData->update(&_defDisplayObjectInstanceStruct, 1);
            _platform->rdDrawGeometry(_oddVertexBufferTextured, _defDisplayObjectInstanceData, platform::PrimitiveTopology::TRIANGLE_STRIP, 4);
        }

        void RenderSupport::drawMesh(const resources::MeshInterface *mesh, const platform::InstanceDataInterface *instanceData) {
            _platform->rdDrawIndexedGeometry(mesh->getMeshBuffer(), instanceData ? instanceData : _defInstanceData, platform::PrimitiveTopology::TRIANGLE_LIST, mesh->getGeometryIndexCount());
        }
        
        void RenderSupport::debugDrawBox(const math::m4x4 &transform, const fg::color &c) {
            _platform->rdSetShader(_simpleShader->getPlatformObject());

            math::p2d offs[5] = {
                math::p2d(-0.5f, -0.5f),
                math::p2d(0.5f, -0.5f),
                math::p2d(0.5f, 0.5f),
                math::p2d(-0.5f, 0.5f),
                math::p2d(-0.5f, -0.5f),
            };

            VertexSimple *tmem = (VertexSimple *)_oddVertexBufferSimple->lock();

            for(int i = 0; i < 4; i++) {
                int ti = i * 6;

                tmem[ti + 0].position.x = offs[i].x;
                tmem[ti + 0].position.y = -0.5f;
                tmem[ti + 0].position.z = offs[i].y;

                tmem[ti + 1].position.x = offs[i + 1].x;
                tmem[ti + 1].position.y = -0.5f;
                tmem[ti + 1].position.z = offs[i + 1].y;

                tmem[ti + 2].position.x = offs[i].x;
                tmem[ti + 2].position.y = -0.5f;
                tmem[ti + 2].position.z = offs[i].y;

                tmem[ti + 3].position.x = offs[i].x;
                tmem[ti + 3].position.y = 0.5f;
                tmem[ti + 3].position.z = offs[i].y;

                tmem[ti + 4].position.x = offs[i].x;
                tmem[ti + 4].position.y = 0.5f;
                tmem[ti + 4].position.z = offs[i].y;

                tmem[ti + 5].position.x = offs[i + 1].x;
                tmem[ti + 5].position.y = 0.5f;
                tmem[ti + 5].position.z = offs[i + 1].y;
            }

            _oddVertexBufferSimple->unlock();

            _defInstanceStruct.modelTransform = transform;
            _defInstanceStruct.rgba = c;
            _defInstanceData->update(&_defInstanceStruct, 1);
            _platform->rdDrawGeometry(_oddVertexBufferSimple, _defInstanceData, platform::PrimitiveTopology::LINE_LIST, 24);
        }

        void RenderSupport::debugDrawBox(const math::p3d &pMin, const math::p3d &pMax, const fg::color &c) {
            _platform->rdSetShader(_simpleShader->getPlatformObject());

            math::p2d offs[5] = {
                math::p2d(pMin.x, pMin.z),
                math::p2d(pMax.x, pMin.z),
                math::p2d(pMax.x, pMax.z),
                math::p2d(pMin.x, pMax.z),
                math::p2d(pMin.x, pMin.z),
            };

            VertexSimple *tmem = (VertexSimple *)_oddVertexBufferSimple->lock();

            for(int i = 0; i < 4; i++) {
                int ti = i * 6;
                tmem[ti + 0].position.x = offs[i].x;
                tmem[ti + 0].position.y = pMin.y;
                tmem[ti + 0].position.z = offs[i].y;

                tmem[ti + 1].position.x = offs[i + 1].x;
                tmem[ti + 1].position.y = pMin.y;
                tmem[ti + 1].position.z = offs[i + 1].y;

                tmem[ti + 2].position.x = offs[i].x;
                tmem[ti + 2].position.y = pMin.y;
                tmem[ti + 2].position.z = offs[i].y;

                tmem[ti + 3].position.x = offs[i].x;
                tmem[ti + 3].position.y = pMax.y;
                tmem[ti + 3].position.z = offs[i].y;

                tmem[ti + 4].position.x = offs[i].x;
                tmem[ti + 4].position.y = pMax.y;
                tmem[ti + 4].position.z = offs[i].y;

                tmem[ti + 5].position.x = offs[i + 1].x;
                tmem[ti + 5].position.y = pMax.y;
                tmem[ti + 5].position.z = offs[i + 1].y;
            }

            _oddVertexBufferSimple->unlock();

            _defInstanceStruct.modelTransform.identity();
            _defInstanceStruct.rgba = c;
            _defInstanceData->update(&_defInstanceStruct, 1);
            _platform->rdDrawGeometry(_oddVertexBufferSimple, _defInstanceData, platform::PrimitiveTopology::LINE_LIST, 24);
        }

        void RenderSupport::debugDrawFillBox(const math::m4x4 &transform, const fg::color &c) {

        }

        void RenderSupport::debugDrawTriangle(const math::p3d &p1, const math::p3d &p2, const math::p3d &p3, const fg::color &c) {
            _platform->rdSetShader(_simpleShader->getPlatformObject());

            VertexNormal *tmem = (VertexNormal *)_oddVertexBufferNormal->lock();

            tmem[0].position.x = p1.x;
            tmem[0].position.y = p1.y;
            tmem[0].position.z = p1.z;

            tmem[1].position.x = p2.x;
            tmem[1].position.y = p2.y;
            tmem[1].position.z = p2.z;

            tmem[2].position.x = p3.x;
            tmem[2].position.y = p3.y;
            tmem[2].position.z = p3.z;

            math::p3d tleft = p2 - p1;
            math::p3d tright = p3 - p1;
            math::p3d tnormal, tbinormal;

            tleft.normalize();
            tright.normalize();
            tnormal.cross(tleft, tright);
            tnormal.normalize();
            tbinormal.cross(tleft, tnormal);
            tbinormal.normalize();

            for(unsigned i = 0; i < 3; i++) {
                tmem[i].normal.x = tnormal.x;
                tmem[i].normal.y = tnormal.y;
                tmem[i].normal.z = tnormal.z;
                tmem[i].binormal.x = tbinormal.x;
                tmem[i].binormal.y = tbinormal.y;
                tmem[i].binormal.z = tbinormal.z;
                tmem[i].tangent.x = tleft.x;
                tmem[i].tangent.y = tleft.y;
                tmem[i].tangent.z = tleft.z;
            }

            _oddVertexBufferNormal->unlock();

            _defInstanceStruct.modelTransform.identity();
            _defInstanceStruct.rgba = c;
            _defInstanceData->update(&_defInstanceStruct, 1);
            _platform->rdDrawGeometry(_oddVertexBufferNormal, _defInstanceData, platform::PrimitiveTopology::TRIANGLE_LIST, 3);
        }

        void RenderSupport::debugDrawLine(const math::p3d &p1, const math::p3d &p2, const fg::color &c) {
            VertexSimple *tmem = (VertexSimple *)_oddVertexBufferSimple->lock();

            tmem[0].position = p1;
            tmem[1].position = p2;

            _oddVertexBufferSimple->unlock();
            _platform->rdSetShader(_simpleShader->getPlatformObject());

            _defInstanceStruct.modelTransform.identity();
            _defInstanceStruct.rgba = c;
            _defInstanceData->update(&_defInstanceStruct, 1);
            _platform->rdDrawGeometry(_oddVertexBufferSimple, _defInstanceData, platform::PrimitiveTopology::LINE_LIST, 2);
        }

        void RenderSupport::debugDrawAxis() {
            debugDrawLine(math::p3d(-1.0f, 0, -1.0f), math::p3d(1.0f, 0, -1.0f), fg::color(0.15f, 0.15f, 0.15f, 1.0f));
            debugDrawLine(math::p3d(1.0f, 0, -1.0f), math::p3d(1.0f, 0, 1.0f), fg::color(0.15f, 0.15f, 0.15f, 1.0f));
            debugDrawLine(math::p3d(1.0f, 0, 1.0f), math::p3d(-1.0f, 0, 1.0f), fg::color(0.15f, 0.15f, 0.15f, 1.0f));
            debugDrawLine(math::p3d(-1.0f, 0, 1.0f), math::p3d(-1.0f, 0, -1.0f), fg::color(0.15f, 0.15f, 0.15f, 1.0f));

            debugDrawLine(math::p3d(), math::p3d(-1000.0f, 0, 0), fg::color(0.3f, 0.3f, 0.3f, 1.0f));
            debugDrawLine(math::p3d(), math::p3d(0, 0, -1000.0f), fg::color(0.3f, 0.3f, 0.3f, 1.0f));

            debugDrawLine(math::p3d(), math::p3d(1000.0f, 0, 0), fg::color(1.0f, 0.0f, 0.0f, 1.0f));
            debugDrawLine(math::p3d(), math::p3d(0, 1000.0f, 0), fg::color(0.0f, 1.0f, 0.0f, 1.0f));
            debugDrawLine(math::p3d(), math::p3d(0, 0, 1000.0f), fg::color(0.0f, 0.0f, 1.0f, 1.0f));
        }
    }
}




//