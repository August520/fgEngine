
namespace fg {
    namespace dx11 {        
        const unsigned __VERTEX_SIZES_MAX = 4;
        const unsigned __INSTANCE_DATA_SIZES_MAX = 2;
        const unsigned __CB_NAMES_MAX = 5;
        const unsigned __LAYOUT_FMT_MAX = 5;
        const unsigned __BUFFER_MAX = 4096;

        unsigned __vertexSizes[__VERTEX_SIZES_MAX] = {
            sizeof(VertexSimple),
            sizeof(VertexTextured),
            sizeof(VertexNormal),
            sizeof(VertexSkinnedNormal),
        };

        unsigned __instanceDataSizes[__INSTANCE_DATA_SIZES_MAX] = {
            sizeof(InstanceDataDefault),
            sizeof(InstanceDataDisplayObject),
        };

        unsigned __texturePixelSizes[] = {4, 1};

        struct NativeLayoutComponent {
            DXGI_FORMAT  format;
            unsigned int size;
        }
        __nativeLayoutFormats[__LAYOUT_FMT_MAX] = {
                {DXGI_FORMAT_UNKNOWN, 0},
                {DXGI_FORMAT_R32_FLOAT, 4},
                {DXGI_FORMAT_R32G32_FLOAT, 8},
                {DXGI_FORMAT_R32G32B32_FLOAT, 12},
                {DXGI_FORMAT_R32G32B32A32_FLOAT, 16},
        };

        char __buffer[__BUFFER_MAX];

        D3D11_TEXTURE_ADDRESS_MODE  __nativeAddrMode[] = {D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_CLAMP};
        D3D11_CULL_MODE             __nativeCullMode[] = {D3D11_CULL_NONE, D3D11_CULL_BACK, D3D11_CULL_FRONT};
        D3D11_COMPARISON_FUNC       __nativeCmpFunc[] = {D3D11_COMPARISON_NEVER, D3D11_COMPARISON_LESS, D3D11_COMPARISON_EQUAL, D3D11_COMPARISON_LESS_EQUAL, D3D11_COMPARISON_GREATER, D3D11_COMPARISON_NOT_EQUAL, D3D11_COMPARISON_GREATER_EQUAL, D3D11_COMPARISON_ALWAYS};
        D3D11_FILTER                __nativeFilter[] = {D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_FILTER_ANISOTROPIC, D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT};
        DXGI_FORMAT                 __nativeTextureFormat[] = {DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_A8_UNORM};
        D3D_PRIMITIVE_TOPOLOGY      __nativeTopology[] = {D3D_PRIMITIVE_TOPOLOGY_LINELIST, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP};

        //---

        UniversalSoundEmitter::UniversalSoundEmitter(UniversalPlatform *owner, unsigned sampleRate, unsigned channels) : PlatformObject(owner), _sndCallback(this) {
            _userCallback = nullptr;
            _userPointer = nullptr;
            _nativeVoice = nullptr;

            WAVEFORMATEX format = {0};
            
            format.wFormatTag = WAVE_FORMAT_PCM;
            format.nChannels = channels;
            format.nSamplesPerSec = sampleRate;
            format.nAvgBytesPerSec = 2 * sampleRate * channels;
            format.nBlockAlign = 2 * channels;
            format.wBitsPerSample = 16;

            _owner->_audio->CreateSourceVoice(&_nativeVoice, &format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &_sndCallback);
        }

        void UniversalSoundEmitter::SoundCallback::OnBufferEnd(void *pBufferContext) {
            if(emitter->_userCallback) {
                emitter->_userCallback(emitter->_userPointer);
            }
        }

        UniversalSoundEmitter::~UniversalSoundEmitter() {

        }

        void UniversalSoundEmitter::pushBuffer(const char *data, unsigned samples) {
            XAUDIO2_BUFFER  bufferInfo = {0};
            bufferInfo.pAudioData = (const BYTE *)data;
            bufferInfo.AudioBytes = 2 * _channels * samples;

            _nativeVoice->SubmitSourceBuffer(&bufferInfo);
        }

        void UniversalSoundEmitter::setBufferEndCallback(void(*cb)(void *), void *userPtr) {
            _userCallback = cb;
            _userPointer = userPtr;
        }

        void UniversalSoundEmitter::setVolume(float volume) {
            _nativeVoice->SetVolume(volume);
        }

        void UniversalSoundEmitter::setWorldTransform(const math::m4x4 &matrix) {

        }

        void UniversalSoundEmitter::play() {
            _nativeVoice->Start();
        }

        void UniversalSoundEmitter::stop() {
            _nativeVoice->Stop();
        }

        void UniversalSoundEmitter::release() {
            if(_nativeVoice) {
                _nativeVoice->DestroyVoice();
            }

            _userCallback = nullptr;
            _nativeVoice = nullptr;

            delete this;
        }

        bool UniversalSoundEmitter::valid() const {
            return _nativeVoice != nullptr;
        }

        //---

        UniversalVertexBuffer::UniversalVertexBuffer(UniversalPlatform *owner, platform::VertexType type, unsigned vcount, bool isDynamic, const void *data) : PlatformObject(owner) {
            D3D11_SUBRESOURCE_DATA  resdata = {0};
            D3D11_BUFFER_DESC       dsc;

            _vcount = vcount;
            _vsize = __vertexSizes[unsigned(type)];
            
            dsc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
            dsc.ByteWidth = _vcount * _vsize;
            dsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            dsc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
            dsc.MiscFlags = 0;

            resdata.pSysMem = data;

            _self = nullptr;
            _owner->_device->CreateBuffer(&dsc, isDynamic ? nullptr : &resdata, &_self);
        }

        UniversalVertexBuffer::~UniversalVertexBuffer() {

        }

        void *UniversalVertexBuffer::lock() {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_self, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            return mapres.pData;
        }

        void UniversalVertexBuffer::unlock() {
            _owner->_context->Unmap(_self, 0);
        }

        void UniversalVertexBuffer::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        ID3D11Buffer *UniversalVertexBuffer::getBuffer() const {
            return _self;
        }

        unsigned UniversalVertexBuffer::getVertexCount() const {
            return _vcount;
        }

        unsigned UniversalVertexBuffer::getVertexSize() const {
            return _vsize;
        }

        bool UniversalVertexBuffer::valid() const {
            return _self != nullptr;
        }

        //--- 

        UniversalIndexedVertexBuffer::UniversalIndexedVertexBuffer(UniversalPlatform *owner, platform::VertexType type, unsigned vcount, unsigned icount, bool isDynamic, const void *vdata, const void *idata) : PlatformObject(owner) {
            D3D11_SUBRESOURCE_DATA  resdata = {0};
            D3D11_BUFFER_DESC       dsc;

            _vcount = vcount;
            _icount = icount;
            _vsize = __vertexSizes[unsigned(type)];
            
            _vbuffer = nullptr;
            _ibuffer = nullptr;

            dsc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
            dsc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
            dsc.MiscFlags = 0;

            dsc.ByteWidth = _vcount * _vsize;
            dsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;            
            resdata.pSysMem = vdata;
            _owner->_device->CreateBuffer(&dsc, isDynamic ? nullptr : &resdata, &_vbuffer);

            dsc.ByteWidth = _icount * sizeof(unsigned short);
            dsc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            resdata.pSysMem = idata;
            _owner->_device->CreateBuffer(&dsc, isDynamic ? nullptr : &resdata, &_ibuffer);
        }

        UniversalIndexedVertexBuffer::~UniversalIndexedVertexBuffer() {
        
        }

        void *UniversalIndexedVertexBuffer::lockVertices() {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_vbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            return mapres.pData;
        }

        void *UniversalIndexedVertexBuffer::lockIndices() {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_ibuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            return mapres.pData;
        }

        void UniversalIndexedVertexBuffer::unlockVertices() {
            _owner->_context->Unmap(_vbuffer, 0);
        }

        void UniversalIndexedVertexBuffer::unlockIndices() {
            _owner->_context->Unmap(_ibuffer, 0);
        }

        void UniversalIndexedVertexBuffer::release() {
            if(_vbuffer) {
                _vbuffer->Release();
            }
            if(_ibuffer) {
                _ibuffer->Release();
            }
            delete this;
        }

        ID3D11Buffer *UniversalIndexedVertexBuffer::getVBuffer() const {
            return _vbuffer;
        }

        ID3D11Buffer *UniversalIndexedVertexBuffer::getIBuffer() const {
            return _ibuffer;
        }

        unsigned UniversalIndexedVertexBuffer::getVertexCount() const {
            return _vcount;
        }

        unsigned UniversalIndexedVertexBuffer::getIndexCount() const {
            return _icount;
        }

        unsigned UniversalIndexedVertexBuffer::getVertexSize() const {
            return _vsize;
        }

        bool UniversalIndexedVertexBuffer::valid() const {
            return _vbuffer != nullptr && _ibuffer != nullptr;
        }

        //---

        UniversalInstanceData::UniversalInstanceData(UniversalPlatform *owner, platform::InstanceDataType type, unsigned instanceCount) : PlatformObject(owner) {
            D3D11_BUFFER_DESC       dsc;

            _instanceDataSize = sizeof(InstanceDataDefault);
            _instanceCount = instanceCount;

            dsc.Usage = D3D11_USAGE_DYNAMIC;
            dsc.ByteWidth = _instanceCount * _instanceDataSize;
            dsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            dsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            dsc.MiscFlags = 0;

            _instanceBuffer = nullptr;
            _owner->_device->CreateBuffer(&dsc, nullptr, &_instanceBuffer);
        }

        UniversalInstanceData::~UniversalInstanceData() {

        }

        void *UniversalInstanceData::lock() {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            return mapres.pData;
        }

        void UniversalInstanceData::unlock() {
            _owner->_context->Unmap(_instanceBuffer, 0);
        }

        void UniversalInstanceData::update(const void *data, unsigned instanceCount) {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);

            if(mapres.pData) {
                memcpy(mapres.pData, data, instanceCount * _instanceDataSize);
            }

            _owner->_context->Unmap(_instanceBuffer, 0);
        }

        void UniversalInstanceData::release() {
            if(_instanceBuffer) {
                _instanceBuffer->Release();
            }
            delete this;
        }

        bool UniversalInstanceData::valid() const {
            return _instanceBuffer != nullptr;
        }

        ID3D11Buffer *UniversalInstanceData::getBuffer() const {
            return _instanceBuffer;
        }

        unsigned UniversalInstanceData::getInstanceDataSize() const {
            return _instanceDataSize;
        }

        //---

        UniversalRasterizerParams::UniversalRasterizerParams(UniversalPlatform *owner, platform::CullMode cull) : PlatformObject(owner) {
            _self = nullptr;

            D3D11_RASTERIZER_DESC rdesc;
            rdesc.FillMode = D3D11_FILL_SOLID;
            rdesc.CullMode = __nativeCullMode[(unsigned int)cull];
            rdesc.FrontCounterClockwise = TRUE;
            rdesc.DepthBias = 0;
            rdesc.SlopeScaledDepthBias = 0.0f;
            rdesc.DepthBiasClamp = 0.0f;
            rdesc.DepthClipEnable = TRUE;
            rdesc.ScissorEnable = TRUE;
            rdesc.MultisampleEnable = FALSE;
            rdesc.AntialiasedLineEnable = FALSE;

            _owner->_device->CreateRasterizerState(&rdesc, &_self);
        }

        UniversalRasterizerParams::~UniversalRasterizerParams() {
        
        }

        void UniversalRasterizerParams::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void UniversalRasterizerParams::set() {
            _owner->_context->RSSetState(_self);
        }

        bool UniversalRasterizerParams::valid() const {
            return _self != nullptr;
        }

        //--- 

        UniversalBlenderParams::UniversalBlenderParams(UniversalPlatform *owner, const platform::BlendMode blendMode) : PlatformObject(owner) {
            _self = nullptr;

            D3D11_BLEND_DESC bdesc;
            bdesc.AlphaToCoverageEnable = FALSE;
            bdesc.IndependentBlendEnable = FALSE;

            bdesc.RenderTarget[0].BlendEnable = blendMode != platform::BlendMode::NOBLEND;
            bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bdesc.RenderTarget[0].DestBlend = blendMode == platform::BlendMode::ALPHA_ADD ? D3D11_BLEND_ONE : D3D11_BLEND_INV_SRC_ALPHA;
            bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            bdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            bdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            bdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            
            _owner->_device->CreateBlendState(&bdesc, &_self);
        }

        UniversalBlenderParams::~UniversalBlenderParams() {
        
        }

        void UniversalBlenderParams::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void UniversalBlenderParams::set() {
            _owner->_context->OMSetBlendState(_self, nullptr, 0xffffffff);
        }

        bool UniversalBlenderParams::valid() const {
            return _self != nullptr;
        }

        //--- 

        UniversalDepthParams::UniversalDepthParams(UniversalPlatform *owner, bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) : PlatformObject(owner) {
            _self = nullptr;

            D3D11_DEPTH_STENCIL_DESC ddesc;
            ddesc.DepthEnable = depthEnabled ? TRUE : FALSE;
            ddesc.DepthWriteMask = depthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            ddesc.DepthFunc = __nativeCmpFunc[(unsigned int)compareFunc];
            ddesc.StencilEnable = FALSE;
            ddesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            ddesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
            ddesc.BackFace.StencilFunc = ddesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            ddesc.BackFace.StencilDepthFailOp = ddesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            ddesc.BackFace.StencilPassOp = ddesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            ddesc.BackFace.StencilFailOp = ddesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

            _owner->_device->CreateDepthStencilState(&ddesc, &_self);
        }

        UniversalDepthParams::~UniversalDepthParams() {
        
        }

        void UniversalDepthParams::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void UniversalDepthParams::set() {
            _owner->_context->OMSetDepthStencilState(_self, 1);
        }

        bool UniversalDepthParams::valid() const {
            return _self != nullptr;
        }

        //--- 
        
        UniversalSampler::UniversalSampler(UniversalPlatform *owner, platform::TextureFilter filter, platform::TextureAddressMode addrMode) : PlatformObject(owner) {
            _self = nullptr;

            D3D11_SAMPLER_DESC sdesc;
            sdesc.Filter = __nativeFilter[(unsigned)filter];
            sdesc.AddressU = __nativeAddrMode[(unsigned int)addrMode];
            sdesc.AddressV = __nativeAddrMode[(unsigned int)addrMode];
            sdesc.AddressW = __nativeAddrMode[(unsigned int)addrMode];
            sdesc.MipLODBias = -0.2f;
            sdesc.MaxAnisotropy = 1;
            sdesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
            sdesc.BorderColor[0] = 1.0f;
            sdesc.BorderColor[1] = 1.0f;
            sdesc.BorderColor[2] = 1.0f;
            sdesc.BorderColor[3] = 1.0f;
            sdesc.MinLOD = -FLT_MAX;
            sdesc.MaxLOD = FLT_MAX;

            if(filter == platform::TextureFilter::SHADOW) {
                sdesc.MipLODBias = 0.0f;
                sdesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
                sdesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
                sdesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            }

            _owner->_device->CreateSamplerState(&sdesc, &_self);
        }

        UniversalSampler::~UniversalSampler() {
        
        }

        void UniversalSampler::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void UniversalSampler::set(platform::TextureSlot slot) {
            _owner->_context->PSSetSamplers(unsigned(slot), 1, &_self);
        }

        bool UniversalSampler::valid() const {
            return _self != nullptr;
        }

        //--- 

        UniversalShader::UniversalShader(UniversalPlatform *owner, const byteinput &binary) : PlatformObject(owner) {
            _vsh = nullptr;
            _psh = nullptr;
            _layout = nullptr;

            binary.readDword(); // received flags

            unsigned  offsetPerVertexData = 0;
            unsigned  offsetPerInstanceData = 0;
            unsigned  inputCount = binary.readDword();
            unsigned  vsLength = binary.readDword();
            unsigned  psLength = binary.readDword();
            char      inputNames[32][32];

            D3D11_INPUT_ELEMENT_DESC inputDesc[32] = {0};

            for(unsigned i = 0; i < inputCount; i++) {
                unsigned short nameLen = binary.readWord();
                binary.readBytes(inputNames[i], nameLen);
                inputNames[i][nameLen] = 0;

                unsigned  semanticIndex = 0;

                char      &semanticIndexCh = inputNames[i][nameLen - 1];
                unsigned  floatCount = binary.readDword();
                unsigned  isPerInstance = binary.readDword();


                if(semanticIndexCh >= '0' && semanticIndexCh <= '9') {
                    semanticIndex = semanticIndexCh - '0';
                    semanticIndexCh = 0;
                }

                inputDesc[i].SemanticName = inputNames[i];
                inputDesc[i].SemanticIndex = semanticIndex;
                inputDesc[i].Format = __nativeLayoutFormats[floatCount].format;
                inputDesc[i].InputSlot = isPerInstance;
                inputDesc[i].AlignedByteOffset = isPerInstance ? offsetPerInstanceData : offsetPerVertexData;
                inputDesc[i].InputSlotClass = isPerInstance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
                inputDesc[i].InstanceDataStepRate = isPerInstance;

                if(isPerInstance) {
                    offsetPerInstanceData += __nativeLayoutFormats[floatCount].size;
                }
                else {
                    offsetPerVertexData += __nativeLayoutFormats[floatCount].size;
                }
            }

            if(_owner->_device->CreateVertexShader(binary.getCurrentPtr(), vsLength, nullptr, &_vsh) == S_OK) {
                if(_owner->_device->CreateInputLayout(inputDesc, inputCount, binary.getCurrentPtr(), vsLength, &_layout) == S_OK) {
                    binary.startOff(binary.getOffset() + vsLength);
                    _owner->_device->CreatePixelShader(binary.getCurrentPtr(), psLength, nullptr, &_psh);
                }
            }
        }

        UniversalShader::~UniversalShader() {
        
        }

        void UniversalShader::release() {
            if(_vsh) {
                _vsh->Release();
            }
            if(_psh) {
                _psh->Release();
            }
            if(_layout) {
                _layout->Release();
            }
            delete this;
        }

        void UniversalShader::set() {
            _owner->_context->IASetInputLayout(_layout);
            _owner->_context->VSSetShader(_vsh, nullptr, 0);
            _owner->_context->PSSetShader(_psh, nullptr, 0);
        }

        bool UniversalShader::valid() const {
            return _psh != nullptr;
        }

        //--- 

        UniversalShaderConstantBuffer::UniversalShaderConstantBuffer(UniversalPlatform *owner, platform::ShaderConstBufferUsing appoint, unsigned byteWidth) : PlatformObject(owner) {
            _self = nullptr;
            _inputIndex = unsigned(appoint);
            _bytewidth = byteWidth;

            D3D11_BUFFER_DESC dsc;
            dsc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DYNAMIC;
            dsc.ByteWidth = byteWidth;
            dsc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            dsc.CPUAccessFlags = 0; //D3D11_CPU_ACCESS_WRITE
            dsc.MiscFlags = 0;

            _owner->_device->CreateBuffer(&dsc, nullptr, &_self);
        }

        UniversalShaderConstantBuffer::~UniversalShaderConstantBuffer() {
        
        }

        void UniversalShaderConstantBuffer::update(const void *data, unsigned byteWidth) {
            D3D11_BOX tbox;
            tbox.back = 1;
            tbox.front = 0;
            tbox.left = 0;
            tbox.right = byteWidth;
            tbox.top = 0;
            tbox.bottom = 1;

            _owner->_context->UpdateSubresource(_self, 0, byteWidth ? &tbox : nullptr, data, 0, 0);
            
            //D3D11_MAPPED_SUBRESOURCE mapres = {0};
            //_owner->_context->Map(_self, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            //
            //if(mapres.pData) {
            //    memcpy(mapres.pData, data, _bytewidth);
            //}

            //_owner->_context->Unmap(_self, 0);
        }

        void UniversalShaderConstantBuffer::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void UniversalShaderConstantBuffer::set() {
            _owner->_context->VSSetConstantBuffers(_inputIndex, 1, &_self);
            
            if(_inputIndex < unsigned(platform::ShaderConstBufferUsing::SKIN_DATA)) {
                _owner->_context->PSSetConstantBuffers(_inputIndex, 1, &_self);
            }
        }

        bool UniversalShaderConstantBuffer::valid() const {
            return _self != nullptr;
        }

        //---

        UniversalTexture2D::UniversalTexture2D() : PlatformObject(nullptr) {
            _self = nullptr;
            _view = nullptr;
            _width = 0;
            _height = 0;
            _mipCount = 0;
            _pixelsz = 0;
        }

        UniversalTexture2D::UniversalTexture2D(UniversalPlatform *owner, unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) : PlatformObject(owner) {
            _self = nullptr;
            _view = nullptr;
            _width = originWidth;
            _height = originHeight;
            _mipCount = mipCount;
            _pixelsz = 4;

            D3D11_TEXTURE2D_DESC      texDesc = {0};
            D3D11_SUBRESOURCE_DATA    subResData[32] = {0};

            texDesc.Width = originWidth;
            texDesc.Height = originHeight;
            texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            texDesc.Usage = D3D11_USAGE_IMMUTABLE;
            texDesc.CPUAccessFlags = 0;
            texDesc.MiscFlags = 0;
            texDesc.MipLevels = mipCount;
            texDesc.ArraySize = 1;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            for(unsigned i = 0; i < mipCount; i++) {
                subResData[i].pSysMem = imgMipsBinaryData[i];
                subResData[i].SysMemPitch = (originWidth >> i) * 4;
                subResData[i].SysMemSlicePitch = 0;
            }

            if(_owner->_device->CreateTexture2D(&texDesc, subResData, &_self) == S_OK) {
                D3D11_SHADER_RESOURCE_VIEW_DESC texViewDesc = {texDesc.Format};
                texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                texViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
                texViewDesc.Texture2D.MostDetailedMip = 0;

                _owner->_device->CreateShaderResourceView(_self, &texViewDesc, &_view);
            }
        }

        UniversalTexture2D::UniversalTexture2D(UniversalPlatform *owner, platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount) : PlatformObject(owner) {
            _self = nullptr;
            _view = nullptr;
            _width = originWidth;
            _height = originHeight;
            _mipCount = mipCount;
            _pixelsz = __texturePixelSizes[unsigned(fmt)];
            
            D3D11_TEXTURE2D_DESC texDesc = {0};
            texDesc.Width = originWidth;
            texDesc.Height = originHeight;
            texDesc.Format = __nativeTextureFormat[unsigned(fmt)];
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.CPUAccessFlags = 0;
            texDesc.MiscFlags = 0;
            texDesc.MipLevels = mipCount;
            texDesc.ArraySize = 1;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            if(_owner->_device->CreateTexture2D(&texDesc, nullptr, &_self) == S_OK) {
                D3D11_SHADER_RESOURCE_VIEW_DESC texViewDesc = {texDesc.Format};
                texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                texViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
                texViewDesc.Texture2D.MostDetailedMip = 0;

                _owner->_device->CreateShaderResourceView(_self, &texViewDesc, &_view);
            }
        }

        UniversalTexture2D::~UniversalTexture2D() {

        }

        unsigned UniversalTexture2D::getWidth() const {
            return _width;
        }

        unsigned UniversalTexture2D::getHeight() const {
            return _height;
        }
        
        unsigned UniversalTexture2D::getMipCount() const {
            return _mipCount;
        }

        void UniversalTexture2D::update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) {
            D3D11_BOX tbox;
            tbox.back = 1;
            tbox.front = 0;
            tbox.left = x;
            tbox.right = x + w;
            tbox.top = y;
            tbox.bottom = y + h;
            _owner->_context->UpdateSubresource(_self, mip, &tbox, src, w * _pixelsz, 0);
        }

        void UniversalTexture2D::release() {
            if(_view) {
                _view->Release();
            }
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        bool UniversalTexture2D::valid() const {
            return _view != nullptr;
        }

        void UniversalTexture2D::set(platform::TextureSlot slot) {
            _owner->_context->PSSetShaderResources(unsigned(slot), 1, &_view);
        }

        //---

        UniversalRenderTarget::UniversalRenderTarget(UniversalPlatform *owner) : PlatformObject(owner) {
            _depthView = nullptr;
            _depthTexture._owner = owner;
            
            for(unsigned i = 0; i < platform::RENDERTARGETS_MAX; i++) {
                _rtView[i] = nullptr;
                _renderTexture[i]._owner = owner;
            }

            _colorTargetCount = 0;
        }

        UniversalRenderTarget::UniversalRenderTarget(UniversalPlatform *owner, unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) : PlatformObject(owner) {
            _depthView = nullptr;
            _colorTargetCount = colorTargetCount;
            _depthTexture._owner = owner;
            
            for(unsigned i = 0; i < platform::RENDERTARGETS_MAX; i++) {
                _rtView[i] = nullptr;
                _renderTexture[i]._owner = owner;
            }

            if(colorTargetCount > platform::RENDERTARGETS_MAX) {
                return;
            }
            
            DXGI_FORMAT  depthTexFormat = DXGI_FORMAT_R24G8_TYPELESS; //DXGI_FORMAT_R32_TYPELESS; dx10
            DXGI_FORMAT  depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT;

            D3D11_TEXTURE2D_DESC  texDesc = {0}; 
            texDesc.Width = originWidth;
            texDesc.Height = originHeight;
            texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.CPUAccessFlags = 0;
            texDesc.MiscFlags = 0;
            texDesc.MipLevels = 1;
            texDesc.ArraySize = 1;
            texDesc.SampleDesc.Count = 1;
            texDesc.SampleDesc.Quality = 0;
            texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

            D3D11_SHADER_RESOURCE_VIEW_DESC texShaderViewDesc = {texDesc.Format};
            texShaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            texShaderViewDesc.Texture2D.MipLevels = 1;
            texShaderViewDesc.Texture2D.MostDetailedMip = 0;

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {texDesc.Format};
            renderTargetViewDesc.Format = texDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            _depthTexture._width = originWidth;
            _depthTexture._height = originHeight;
            _depthTexture._mipCount = 1;
            _depthTexture._pixelsz = 4;

            for(unsigned i = 0; i < _colorTargetCount; i++) {
                _renderTexture[i]._width = originWidth;
                _renderTexture[i]._height = originHeight;
                _renderTexture[i]._mipCount = 1;
                _renderTexture[i]._pixelsz = 4;

                if(_owner->_device->CreateTexture2D(&texDesc, nullptr, &_renderTexture[i]._self) == S_OK) {
                    _owner->_device->CreateRenderTargetView(_renderTexture[i]._self, &renderTargetViewDesc, &_rtView[i]);                    
                    _owner->_device->CreateShaderResourceView(_renderTexture[i]._self, &texShaderViewDesc, &_renderTexture[i]._view);
                }
            }

            D3D11_TEXTURE2D_DESC depthTexDesc = {0};
            depthTexDesc.Width = originWidth;
            depthTexDesc.Height = originHeight;
            depthTexDesc.MipLevels = 1;
            depthTexDesc.ArraySize = 1;
            depthTexDesc.Format = depthTexFormat;
            depthTexDesc.SampleDesc.Count = 1;
            depthTexDesc.SampleDesc.Quality = 0;
            depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
            depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
            depthTexDesc.CPUAccessFlags = 0;
            depthTexDesc.MiscFlags = 0;

            if(_owner->_device->CreateTexture2D(&depthTexDesc, 0, &_depthTexture._self) == S_OK) {
                D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {depthFormat};
                depthDesc.Format = depthFormat;
                depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                depthDesc.Texture2D.MipSlice = 0;

                D3D11_SHADER_RESOURCE_VIEW_DESC depthShaderViewDesc = {DXGI_FORMAT_R24_UNORM_X8_TYPELESS};
                depthShaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                depthShaderViewDesc.Texture2D.MipLevels = 1; //!
                depthShaderViewDesc.Texture2D.MostDetailedMip = 0;

                _owner->_device->CreateDepthStencilView(_depthTexture._self, &depthDesc, &_depthView);
                _owner->_device->CreateShaderResourceView(_depthTexture._self, &depthShaderViewDesc, &_depthTexture._view);
            }
        }

        UniversalRenderTarget::~UniversalRenderTarget() {

        }

        platform::Texture2DInterface *UniversalRenderTarget::getDepthBuffer() {
            return &_depthTexture;
        }

        platform::Texture2DInterface *UniversalRenderTarget::getRenderBuffer(unsigned index) {
            return &_renderTexture[index]; 
        }

        unsigned UniversalRenderTarget::getRenderBufferCount() const {
            return _colorTargetCount;
        }

        void UniversalRenderTarget::release() {
            if(_depthView) {
                _depthView->Release();
                _depthTexture._self->Release();
                _depthTexture._view->Release();
            }

            for(unsigned i = 0; i < platform::RENDERTARGETS_MAX; i++) {
                if(_renderTexture[i]._self) {
                    _renderTexture[i]._self->Release();
                    _renderTexture[i]._view->Release();
                }

                if(_rtView[i]) {
                    _rtView[i]->Release();
                }
            }

            delete this;
        }

        void UniversalRenderTarget::set() {            
            D3D11_VIEWPORT vp;
            vp.TopLeftX = vp.TopLeftY = 0;
            vp.Width = float(_renderTexture->_width);
            vp.Height = float(_renderTexture->_height);
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;

            D3D11_RECT rect;
            rect.top = 0;
            rect.left = 0;
            rect.right = int(_renderTexture->_width) - 1;
            rect.bottom = int(_renderTexture->_height) - 1;

            _owner->_context->RSSetScissorRects(1, &rect);
            
            _owner->_context->OMSetRenderTargets(_colorTargetCount, _rtView, _depthView);
            _owner->_context->RSSetViewports(1, &vp);
        }
        
        bool UniversalRenderTarget::valid() const {
            if(_depthView == nullptr) {
                return false;
            }

            for(unsigned i = 0; i < _colorTargetCount; i++) {
                if(_rtView[i] == nullptr) {
                    return false;
                }
            }

            return true;
        }

        //---
        
        UniversalPlatform::UniversalPlatform(const diag::LogInterface &log) : _log(log), _defRenderTarget(this) {
            _device = nullptr;
            _context = nullptr;
            _swapChain = nullptr;
            _defSampler = nullptr;
            _nativeWidth = 0.0f;
            _nativeHeight = 0.0f;
            _syncInterval = 0;
            _curRenderTarget = nullptr;
        }
        
        bool UniversalPlatform::init(const platform::InitParams &initParams) {
            UniversalInitParams &params = (UniversalInitParams &)initParams;
            //-- sound

            if(XAudio2Create(&_audio, 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK) {
                if(_audio->CreateMasteringVoice(&_mastering) != S_OK) {
                    _log.msgError("can't create mastering voice");
                    return false;
                }
            }
            else {
                _log.msgError("can't create sound device");
                return false;
            }

            //-- graphics
            _initDevice();

            _orientation = params.orientation;
            _nativeWidth = params.scrWidth;
            _nativeHeight = params.scrHeight;
            _syncInterval = params.syncInterval;
            _window = params.window;
            _swapChainPanel = params.swapChainPanel;

            DisplayInformation ^curDisplayInfo = DisplayInformation::GetForCurrentView();

            if(params.orientation == platform::Orientation::ALBUM) {
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::Portrait || curDisplayInfo->CurrentOrientation == DisplayOrientations::PortraitFlipped) {
                    _nativeWidth = params.scrHeight;
                    _nativeHeight = params.scrWidth;
                }
            }
            else {
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::Landscape || curDisplayInfo->CurrentOrientation == DisplayOrientations::LandscapeFlipped) {
                    _nativeWidth = params.scrHeight;
                    _nativeHeight = params.scrWidth;
                }
            }

            IDXGIAdapter  *adapter;
            IDXGIDevice1  *dxgiDevice = 0;
            IDXGIFactory2 *dxgiFactory;
            _device->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice);

            dxgiDevice->GetAdapter(&adapter);
            adapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory);
            adapter->Release();

            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
            swapChainDesc.Width = int(_nativeWidth);
            swapChainDesc.Height = int(_nativeHeight);
            swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 2;
            swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            swapChainDesc.Flags = 0;

            if (_window.Get() != nullptr) {
                if(dxgiFactory->CreateSwapChainForCoreWindow(_device, (IUnknown *)params.window.Get(), &swapChainDesc, nullptr, &_swapChain) != S_OK) {
                    _log.msgError("can't create hardware device");
                    return false;
                }
            }
            else if(_swapChainPanel.Get() != nullptr) {
                if (dxgiFactory->CreateSwapChainForComposition(_device, &swapChainDesc, nullptr, &_swapChain) != S_OK) {
                    _log.msgError("can't create hardware device");
                    return false;
                }

                IDXGISwapChain2 *swapChain2 = nullptr;
                ISwapChainPanelNative *swapChainPanelNative = nullptr;
                IInspectable *panelInspectable = (IInspectable *) reinterpret_cast<IInspectable *>(_swapChainPanel.Get());
                panelInspectable->QueryInterface(__uuidof(ISwapChainPanelNative), (void **)&swapChainPanelNative);

                swapChainPanelNative->SetSwapChain(_swapChain);
                swapChainPanelNative->Release();

                _swapChain->QueryInterface(&swapChain2);

                DXGI_MATRIX_3X2_F scale = {0};
                scale._11 = 1.0f / _swapChainPanel->CompositionScaleX;
                scale._22 = 1.0f / _swapChainPanel->CompositionScaleY;

                swapChain2->SetMatrixTransform(&scale);
                swapChain2->Release();
            }
            else return false;
            
            dxgiFactory->Release();
            dxgiDevice->SetMaximumFrameLatency(1);
            dxgiDevice->Release();

            updateOrientation();

            //---

            _defRenderTarget._owner = this;
            _initDefaultRenderTarget();

            _defSampler = new UniversalSampler(this, platform::TextureFilter::LINEAR, platform::TextureAddressMode::CLAMP);

            for (unsigned i = 0; i < platform::TEXTURE_UNITS_MAX; i++) {
                _lastTextureWidth[i] = 0.0f;
                _lastTextureHeight[i] = 0.0f;
                rdSetSampler(platform::TextureSlot(i), _defSampler);
            }
            return true;
        }

        void UniversalPlatform::destroy() {  
            ID3D11RenderTargetView *tt[] = {nullptr};
            _context->OMSetRenderTargets(1, tt, nullptr);
            _context->Flush();
            _context->ClearState();
            
            _defSampler->release();
            _defRenderTarget._depthTexture._view->Release();
            _defRenderTarget._depthView->Release();
            _defRenderTarget._rtView[0]->Release();
            _swapChain->Release();
            _context->Release();
            _device->Release();
            _defSampler = nullptr;
            _defRenderTarget._depthTexture._view = nullptr;
            _defRenderTarget._depthView = nullptr;
            _defRenderTarget._rtView[0] = nullptr;
            _swapChain = nullptr;
            _context = nullptr;
            _device = nullptr;

            //pDevice->QueryInterface(IID_PPV_ARGS(&pDebug));

            _mastering->DestroyVoice();
            _mastering = nullptr;
            _audio->Release();
            _audio = nullptr;
        }

        void UniversalPlatform::resize(float width, float height) {
            DisplayInformation ^curDisplayInfo = DisplayInformation::GetForCurrentView();

            _nativeWidth = width;
            _nativeHeight = height;

            if (_orientation == platform::Orientation::ALBUM) {
                if (curDisplayInfo->CurrentOrientation == DisplayOrientations::Portrait || curDisplayInfo->CurrentOrientation == DisplayOrientations::PortraitFlipped) {
                    _nativeWidth = height;
                    _nativeHeight = width;
                }
            }
            else {
                if (curDisplayInfo->CurrentOrientation == DisplayOrientations::Landscape || curDisplayInfo->CurrentOrientation == DisplayOrientations::LandscapeFlipped) {
                    _nativeWidth = height;
                    _nativeHeight = width;
                }
            }
            
            _defRenderTarget._rtView[0]->Release();
            _defRenderTarget._depthTexture._view->Release();
            _defRenderTarget._depthView->Release();
            _context->OMSetRenderTargets(0, 0, 0);

            _swapChain->ResizeBuffers(2, unsigned(_nativeWidth), unsigned(_nativeHeight), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
            _initDefaultRenderTarget();
        }

        void UniversalPlatform::_initDevice() {
            if (_context) {
                _context->Release();
            }
            if (_device) {                
                _device->Release();
            }

            ID3D11DeviceContext  *tcontext;
            ID3D11Device         *tdevice;
            D3D_FEATURE_LEVEL    featureLevel;

            unsigned flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
            //unsigned flags = D3D11_CREATE_DEVICE_DEBUG; //D3D11_CREATE_DEVICE_DEBUG; //| D3D11_CREATE_DEVICE_BGRA_SUPPORT

            D3D_FEATURE_LEVEL features[] = {
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1
            };

            D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, features, 3, D3D11_SDK_VERSION, &tdevice, &featureLevel, &tcontext);

            tdevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&_device);
            tcontext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void **)&_context);
            tdevice->Release();
            tcontext->Release();
        }
        
        void UniversalPlatform::_initDefaultRenderTarget() {
            ID3D11Texture2D *tmptex;
            _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&tmptex);

            if (_device->CreateRenderTargetView(tmptex, 0, &_defRenderTarget._rtView[0]) != S_OK) {
                _swapChain->Release();
                _context->Release();
                _device->Release();
                _swapChain = nullptr;
                _context = nullptr;
                _device = nullptr;
                _log.msgError("can't create default render target");
                return;
            }

            tmptex->Release();

            DXGI_FORMAT depthTexFormat = DXGI_FORMAT_R24G8_TYPELESS; //DXGI_FORMAT_R32_TYPELESS; dx10
            DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT;
            UINT        depthTexBind = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

            D3D11_TEXTURE2D_DESC depthTexDesc = { 0 };
            depthTexDesc.Width = int(_nativeWidth);
            depthTexDesc.Height = int(_nativeHeight);
            depthTexDesc.MipLevels = 1;
            depthTexDesc.ArraySize = 1;
            depthTexDesc.Format = depthTexFormat;
            depthTexDesc.SampleDesc.Count = 1;
            depthTexDesc.SampleDesc.Quality = 0;
            depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
            depthTexDesc.BindFlags = depthTexBind;
            depthTexDesc.CPUAccessFlags = 0;
            depthTexDesc.MiscFlags = 0;

            D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = { depthFormat };
            depthDesc.Format = depthFormat;
            depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depthDesc.Texture2D.MipSlice = 0;

            if (_device->CreateTexture2D(&depthTexDesc, 0, &tmptex) != S_OK) {
                _defRenderTarget._rtView[0]->Release();
                _defRenderTarget._rtView[0] = nullptr;
                _swapChain->Release();
                _context->Release();
                _device->Release();
                _swapChain = nullptr;
                _context = nullptr;
                _device = nullptr;
                _log.msgError("can't create zbuffer texture");
                return;
            }

            _device->CreateDepthStencilView(tmptex, &depthDesc, &_defRenderTarget._depthView);

            D3D11_SHADER_RESOURCE_VIEW_DESC texViewDesc = { DXGI_FORMAT_R24_UNORM_X8_TYPELESS };
            texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            texViewDesc.Texture2D.MipLevels = 1;
            texViewDesc.Texture2D.MostDetailedMip = 0;

            _device->CreateShaderResourceView(tmptex, &texViewDesc, &_defRenderTarget._depthTexture._view);
            tmptex->Release();

            _defRenderTarget._depthTexture._width = unsigned(_nativeWidth);
            _defRenderTarget._depthTexture._height = unsigned(_nativeHeight);
            _defRenderTarget._depthTexture._mipCount = 1;
            _defRenderTarget._renderTexture[0]._width = unsigned(_nativeWidth);
            _defRenderTarget._renderTexture[0]._height = unsigned(_nativeHeight);
            _defRenderTarget._renderTexture[0]._mipCount = 1;
            _defRenderTarget._colorTargetCount = 1;
            _curRenderTarget = &_defRenderTarget;

            D3D11_VIEWPORT vp;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;
            vp.Width = _nativeWidth;
            vp.Height = _nativeHeight;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            _context->RSSetViewports(1, &vp);

            D3D11_RECT rect;
            rect.top = 0;
            rect.left = 0;
            rect.right = int(_nativeWidth) - 1;
            rect.bottom = int(_nativeHeight) - 1;
            _context->RSSetScissorRects(1, &rect);
        }
        
        float UniversalPlatform::getScreenWidth() const {
            return _nativeWidth;
        }

        float UniversalPlatform::getScreenHeight() const {
            return _nativeHeight;
        }

        float UniversalPlatform::getCurrentRTWidth() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getWidth());
        }

        float UniversalPlatform::getCurrentRTHeight() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getHeight());
        }

        float UniversalPlatform::getTextureWidth(platform::TextureSlot slot) const {
            return _lastTextureWidth[unsigned(slot)];
        }

        float UniversalPlatform::getTextureHeight(platform::TextureSlot slot) const {
            return _lastTextureHeight[unsigned(slot)];
        }

        const math::m3x3 &UniversalPlatform::getInputTransform() const {
            return _inputTransform;
        }

        unsigned UniversalPlatform::getMemoryUsing() const {
            return unsigned(Windows::System::MemoryManager::AppMemoryUsage / 1024);
        }

        unsigned UniversalPlatform::getMemoryLimit() const {
            return unsigned(Windows::System::MemoryManager::AppMemoryUsageLimit / 1024);
        }

        unsigned  long long UniversalPlatform::getTimeMs() const {
            unsigned __int64 ttime;
            GetSystemTimeAsFileTime((FILETIME *)&ttime);
            return ttime / 10000;
        }

        void UniversalPlatform::updateOrientation() {
            DXGI_MODE_ROTATION rotationMode;
            DisplayInformation ^curDisplayInfo = DisplayInformation::GetForCurrentView();

            _swapChain->GetRotation(&rotationMode);

            if(_orientation == platform::Orientation::ALBUM) {
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::Portrait) {
                    _inputTransform.setRotate(3.0f * M_PI / 2.0f);
                    _inputTransform._32 = _nativeHeight;
                    rotationMode = DXGI_MODE_ROTATION_ROTATE270;
                }
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::PortraitFlipped) {
                    _inputTransform.setRotate(M_PI / 2.0f);
                    _inputTransform._31 = _nativeWidth;
                    rotationMode = DXGI_MODE_ROTATION_ROTATE90;
                }
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::Landscape) {
                    _inputTransform.identity();                    
                    rotationMode = DXGI_MODE_ROTATION_IDENTITY;
                }
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::LandscapeFlipped) {
                    _inputTransform.identity();                    
                    rotationMode = DXGI_MODE_ROTATION_IDENTITY;
                }
            }
            else {
                if (curDisplayInfo->CurrentOrientation == DisplayOrientations::Portrait) {
                    _inputTransform.identity();
                    rotationMode = DXGI_MODE_ROTATION_IDENTITY;
                }
                if (curDisplayInfo->CurrentOrientation == DisplayOrientations::PortraitFlipped) {
                    _inputTransform.identity();
                    rotationMode = DXGI_MODE_ROTATION_IDENTITY;
                }
                if (curDisplayInfo->CurrentOrientation == DisplayOrientations::Landscape) {
                    _inputTransform.setRotate(M_PI / 2.0f);
                    _inputTransform._31 = _nativeWidth;
                    rotationMode = DXGI_MODE_ROTATION_ROTATE90;
                }
                if (curDisplayInfo->CurrentOrientation == DisplayOrientations::LandscapeFlipped) {
                    _inputTransform.setRotate(3.0f * M_PI / 2.0f);
                    _inputTransform._32 = _nativeHeight;
                    rotationMode = DXGI_MODE_ROTATION_ROTATE270;
                }
            }

            _swapChain->SetRotation(rotationMode);
        }       

        void UniversalPlatform::fsFormFilesList(const char *path, std::string &out) {
            struct fn {
                static void formListW(const wchar_t *pathw, std::string &out) {
                    try {
                        Windows::Storage::StorageFolder ^fldr = create_task(Windows::ApplicationModel::Package::Current->InstalledLocation->GetFolderAsync(ref new Platform::String(pathw))).get();
                        Windows::Foundation::Collections::IVectorView <StorageFile ^> ^files = create_task(fldr->GetFilesAsync()).get();
                        Windows::Foundation::Collections::IVectorView <StorageFolder ^> ^folders = create_task(fldr->GetFoldersAsync()).get();

                        for(unsigned int i = 0; i < folders->Size; i++) {
                            Windows::Storage::StorageFolder ^curFolder = folders->GetAt(i);

                            unsigned int  toff = 0;
                            wchar_t       tpath[260];
                            
                            wcscpy_s(tpath, pathw);
                            wcscat_s(tpath, L"\\");
                            wcscat_s(tpath, curFolder->Name->Data());

                            formListW(tpath, out);
                        }

                        for(unsigned int i = 0; i < files->Size; i++) {
                            Windows::Storage::StorageFile ^curFile = files->GetAt(i);

                            unsigned int converted1 = 0;
                            unsigned int converted2 = 0;
                            char         tpathc[256] = {0};
                            wcstombs_s(&converted1, tpathc, pathw, wcslen(pathw));

                            tpathc[converted1 - 1] = '\\';
                            converted1++;
                            //
                            wcstombs_s(&converted2, tpathc + converted1 - 1, 255 - converted1, curFile->Name->Data(), curFile->Name->Length());
                            tpathc[converted1 + converted2] = 0;

                            out += tpathc;
                            out += "\n";
                        }
                    }
                    catch(Exception ^ex) {}
                }
            };

            wchar_t     pathw[260];            
            unsigned    wcount = 0;
            const char  *source = path;

            while(*source != 0) {
                unsigned chlen = 1;
                wchar_t  ch = string::utf8ToUTF16(source, &chlen);

                pathw[wcount++] = ch == '/' ? '\\' : ch;
                source += chlen;
            }

            pathw[wcount++] = 0;
            fn::formListW(pathw, out);
        }

        bool UniversalPlatform::fsLoadFile(const char *path, void **oBinaryDataPtr, unsigned int *oSize) {
            wchar_t loadPathW[260];

            const char *source = path;
            unsigned    wcount = 0;

            while(*source != 0) {
                unsigned chlen = 1;
                wchar_t  ch = string::utf8ToUTF16(source, &chlen);

                loadPathW[wcount++] = ch == '/' ? '\\' : ch;
                source += chlen;
            }

            loadPathW[wcount++] = 0;
            auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

            try {
                StorageFile ^file = create_task(folder->GetFileAsync(ref new String(loadPathW))).get();
                Streams::IRandomAccessStream ^stream = create_task(file->OpenAsync(FileAccessMode::Read)).get();
                Streams::DataReader ^reader = ref new Streams::DataReader(stream);

                unsigned bytesLoaded = create_task(reader->LoadAsync((unsigned)stream->Size)).get();
                *oBinaryDataPtr = new char [bytesLoaded];
                *oSize = bytesLoaded;
                reader->ReadBytes(Platform::ArrayReference <unsigned char>((unsigned char *)*oBinaryDataPtr, bytesLoaded, false));
                return true;
            }
            catch(Exception ^ex) {
                return false;
            }
        }

        // not tested
        bool UniversalPlatform::fsSaveFile(const char *path, void *iBinaryDataPtr, unsigned iSize) {
            wchar_t loadPathW[260];

            const char *source = path;
            unsigned    wcount = 0;

            while(*source != 0) {
                unsigned chlen = 1;
                wchar_t  ch = string::utf8ToUTF16(source, &chlen);

                loadPathW[wcount++] = ch == '/' ? '\\' : ch;
                source += chlen;
            }

            loadPathW[wcount++] = 0;
            auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

            try {
                StorageFile ^file = create_task(folder->CreateFileAsync(ref new String(loadPathW))).get();
                Streams::IRandomAccessStream ^stream = create_task(file->OpenAsync(FileAccessMode::ReadWrite)).get();
                Streams::DataWriter ^writer = ref new Streams::DataWriter (stream);

                writer->WriteBytes(Platform::ArrayReference <unsigned char> ((unsigned char *)iBinaryDataPtr, iSize, false));
                create_task(writer->StoreAsync()).get();

                return true;
            }
            catch(Exception ^ex) {
                return false;
            }
        }

        void UniversalPlatform::sndSetGlobalVolume(float volume) {
            _mastering->SetVolume(volume);
        }

        platform::SoundEmitterInterface *UniversalPlatform::sndCreateEmitter(unsigned sampleRate, unsigned channels) {
            UniversalSoundEmitter *r = new UniversalSoundEmitter (this, sampleRate, channels);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create sound emitter");
                delete r;
                return nullptr;
            }
        }

        platform::VertexBufferInterface *UniversalPlatform::rdCreateVertexBuffer(platform::VertexType vtype, unsigned vcount, bool isDynamic, const void *data) {
            UniversalVertexBuffer *r = new UniversalVertexBuffer (this, vtype, vcount, isDynamic, data);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create vertex buffer");
                delete r;
                return nullptr;
            }
        }

        platform::IndexedVertexBufferInterface *UniversalPlatform::rdCreateIndexedVertexBuffer(platform::VertexType vtype, unsigned vcount, unsigned ushortIndexCount, bool isDynamic, const void *vdata, const void *idata) {
            UniversalIndexedVertexBuffer *r = new UniversalIndexedVertexBuffer (this, vtype, vcount, ushortIndexCount, isDynamic, vdata, idata);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create indexed vertex buffer");
                delete r;
                return nullptr;
            }
        }

        platform::InstanceDataInterface *UniversalPlatform::rdCreateInstanceData(platform::InstanceDataType type, unsigned instanceCount) {
            UniversalInstanceData *r = new UniversalInstanceData(this, type, instanceCount);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create instance buffer");
                delete r;
                return nullptr;
            }

        }

        platform::ShaderInterface *UniversalPlatform::rdCreateShader(const byteinput &binary) {
            UniversalShader *r = new UniversalShader (this, binary);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create shader");
                delete r;
                return nullptr;
            }
        }
        
        platform::RasterizerParamsInterface *UniversalPlatform::rdCreateRasterizerParams(platform::CullMode cull) {
            UniversalRasterizerParams *r = new UniversalRasterizerParams (this, cull);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create rasterizer state");
                delete r;
                return nullptr;
            }
        }

        platform::BlenderParamsInterface *UniversalPlatform::rdCreateBlenderParams(const platform::BlendMode blendMode) {
            UniversalBlenderParams *r = new UniversalBlenderParams (this, blendMode);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create blender state");
                delete r;
                return nullptr;
            }
        }

        platform::DepthParamsInterface *UniversalPlatform::rdCreateDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) {
            UniversalDepthParams *r = new UniversalDepthParams (this, depthEnabled, compareFunc, depthWriteEnabled);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create depth-stencil state");
                delete r;
                return nullptr;
            }
        }

        platform::SamplerInterface *UniversalPlatform::rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode) {
            UniversalSampler *r = new UniversalSampler (this, filter, addrMode);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create sampler state");
                delete r;
                return nullptr;
            }
        }

        platform::ShaderConstantBufferInterface *UniversalPlatform::rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) {
            UniversalShaderConstantBuffer *r = new UniversalShaderConstantBuffer (this, appoint, byteWidth);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create shader constant buffer");
                delete r;
                return nullptr;
            }
        }

        platform::Texture2DInterface *UniversalPlatform::rdCreateTexture2D(unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            UniversalTexture2D *r = new UniversalTexture2D(this, imgMipsBinaryData, originWidth, originHeight, mipCount);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("can't create texture2d from memory");
                delete r;
                return nullptr;
            }
            
            return nullptr;
        }

        platform::Texture2DInterface *UniversalPlatform::rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            UniversalTexture2D *r = new UniversalTexture2D (this, format, originWidth, originHeight, mipCount);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("can't create texture2d with format");
                delete r;
                return nullptr;
            }
        }

        platform::RenderTargetInterface *UniversalPlatform::rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) {
            UniversalRenderTarget *r = new UniversalRenderTarget (this, colorTargetCount, originWidth, originHeight);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("can't create render target");
                delete r;
                return nullptr;
            }
        }

        platform::RenderTargetInterface *UniversalPlatform::rdGetDefaultRenderTarget() {
            return &_defRenderTarget;
        }

        void UniversalPlatform::rdClearCurrentDepthBuffer(float depth) {
            _context->ClearDepthStencilView(_curRenderTarget->_depthView, D3D11_CLEAR_DEPTH, depth, 0);
        }

        void UniversalPlatform::rdClearCurrentColorBuffer(const fg::color &c) {
            for(unsigned i = 0; i < _curRenderTarget->_colorTargetCount; i++) {
                _context->ClearRenderTargetView(_curRenderTarget->_rtView[i], (float *)&c);
            }
        }

        void UniversalPlatform::rdSetRenderTarget(const platform::RenderTargetInterface *rt) {
            UniversalRenderTarget *dxObject = (UniversalRenderTarget *)rt;
            _curRenderTarget = dxObject;
            dxObject->set();            
        }

        void UniversalPlatform::rdSetShader(const platform::ShaderInterface *shader) {
            UniversalShader *dxObject = (UniversalShader *)shader;
            dxObject->set();
        }

        void UniversalPlatform::rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) {
            UniversalRasterizerParams *dxObj = (UniversalRasterizerParams *)params;
            dxObj->set();
        }

        void UniversalPlatform::rdSetBlenderParams(const platform::BlenderParamsInterface *params) {
            UniversalBlenderParams *dxObj = (UniversalBlenderParams *)params;
            dxObj->set();
        }

        void UniversalPlatform::rdSetDepthParams(const platform::DepthParamsInterface *params) {
            UniversalDepthParams *dxObj = (UniversalDepthParams *)params;
            dxObj->set();
        }

        void UniversalPlatform::rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) {
            UniversalSampler *dxObj = (UniversalSampler *)sampler;
            dxObj->set(slot);
        }

        void UniversalPlatform::rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) {
            UniversalShaderConstantBuffer *dxObj = (UniversalShaderConstantBuffer *)cbuffer;
            dxObj->set();
        }

        void UniversalPlatform::rdSetTexture2D(platform::TextureSlot slot, const platform::Texture2DInterface *texture) {
            if(texture) {
                UniversalTexture2D *dxObj = (UniversalTexture2D *)texture;
                dxObj->set(slot);
                _lastTextureWidth[unsigned(slot)] = float(dxObj->getWidth());
                _lastTextureHeight[unsigned(slot)] = float(dxObj->getHeight());
            }
            else {
                ID3D11ShaderResourceView *tnull = nullptr;
                _context->PSSetShaderResources(unsigned(slot), 1, &tnull);
            }
        }

        void UniversalPlatform::rdSetScissorRect(math::p2d &topLeft, math::p2d &bottomRight) {
            D3D11_RECT rect;
            rect.left = int(topLeft.x);
            rect.top = int(topLeft.y);
            rect.right = int(bottomRight.x);
            rect.bottom = int(bottomRight.y);
            _context->RSSetScissorRects(1, &rect);
        }

        void UniversalPlatform::rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned vertexCount, unsigned instanceCount) {
            UniversalVertexBuffer *dxVB = (UniversalVertexBuffer *)vbuffer;
            UniversalInstanceData *dxInstanceData = (UniversalInstanceData *)instanceData;

            unsigned int offsets[2] = {0, 0};
            unsigned int strides[2] = {dxVB->getVertexSize(), dxInstanceData->getInstanceDataSize()};
            ID3D11Buffer *buffers[2] = {dxVB->getBuffer(), dxInstanceData->getBuffer()};

            _context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
            _context->IASetPrimitiveTopology(__nativeTopology[(unsigned int)topology]);
            _context->Draw(vertexCount, 0); // instancing draw now not working on windows Universal 
        }

        void UniversalPlatform::rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned indexCount, unsigned instanceCount) {
            UniversalIndexedVertexBuffer *dxIVB = (UniversalIndexedVertexBuffer *)ivbuffer;
            UniversalInstanceData *dxInstanceData = (UniversalInstanceData *)instanceData;

            unsigned int offsets[2] = {0, 0};
            unsigned int strides[2] = {dxIVB->getVertexSize(), dxInstanceData->getInstanceDataSize()};
            ID3D11Buffer *buffers[2] = {dxIVB->getVBuffer(), dxInstanceData->getBuffer()};
            ID3D11Buffer *indexBuff = dxIVB->getIBuffer();

            _context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
            _context->IASetIndexBuffer(indexBuff, DXGI_FORMAT_R16_UINT, 0);
            _context->IASetPrimitiveTopology(__nativeTopology[(unsigned int)topology]);
            _context->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0); 
        }

        void UniversalPlatform::rdPresent() {
            if(_swapChain) {
                if(_swapChain->Present(_syncInterval, 0) == DXGI_ERROR_DEVICE_REMOVED) {
                    //
                }
                else {
                    static ID3D11ShaderResourceView *tnull[platform::TEXTURE_UNITS_MAX] = {nullptr};
                    _context->PSSetShaderResources(0, platform::TEXTURE_UNITS_MAX, tnull);
                }
            }            
        }

        bool UniversalPlatform::isInited() const {
            return _device != nullptr;
        }
    }
}




