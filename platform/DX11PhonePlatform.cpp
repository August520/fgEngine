
namespace fg {
    namespace dx11 {        
        const unsigned __VERTEX_SIZES_MAX      = 5;
        const unsigned __CB_NAMES_MAX          = 5;
        const unsigned __LAYOUT_FMT_MAX        = 5;
        const unsigned __BUFFER_MAX            = 4096;
        
        unsigned __vertexSizes[__VERTEX_SIZES_MAX] = {
            3 * sizeof(float), 
            5 * sizeof(float), 
            14 * sizeof(float), 
            13 * sizeof(float), 
            22 * sizeof(float),
        };

        unsigned __texturePixelSizes[] = {4, 1, 1};

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

        const char *__cbNames[__CB_NAMES_MAX] = {
            "FrameData",
            "DrawData",
            "MaterialData",
            "SkinData",
            "AdditionalData",
        };

        char __buffer[__BUFFER_MAX];

        D3D11_TEXTURE_ADDRESS_MODE  __nativeAddrMode[] = {D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_CLAMP};
        D3D11_CULL_MODE             __nativeCullMode[] = {D3D11_CULL_NONE, D3D11_CULL_BACK, D3D11_CULL_FRONT};
        D3D11_COMPARISON_FUNC       __nativeCmpFunc[] = {D3D11_COMPARISON_NEVER, D3D11_COMPARISON_LESS, D3D11_COMPARISON_EQUAL, D3D11_COMPARISON_LESS_EQUAL, D3D11_COMPARISON_GREATER, D3D11_COMPARISON_NOT_EQUAL, D3D11_COMPARISON_GREATER_EQUAL, D3D11_COMPARISON_ALWAYS};
        D3D11_FILTER                __nativeFilter[] = {D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_FILTER_ANISOTROPIC, D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT};
        DXGI_FORMAT                 __nativeTextureFormat[] = {DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_A8_UNORM};
        D3D_PRIMITIVE_TOPOLOGY      __nativeTopology[] = {D3D_PRIMITIVE_TOPOLOGY_LINELIST, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP};

        //---

        PhoneSoundEmitter::PhoneSoundEmitter(PhonePlatform *owner, unsigned sampleRate, unsigned channels) : PlatformObject(owner), _sndCallback(this) {
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

        void PhoneSoundEmitter::SoundCallback::OnBufferEnd(void *pBufferContext) {
            if(emitter->_userCallback) {
                emitter->_userCallback(emitter->_userPointer);
            }
        }

        PhoneSoundEmitter::~PhoneSoundEmitter() {

        }

        void PhoneSoundEmitter::pushBuffer(const char *data, unsigned samples) {
            XAUDIO2_BUFFER  bufferInfo = {0};
            bufferInfo.pAudioData = (const BYTE *)data;
            bufferInfo.AudioBytes = 2 * _channels * samples;

            _nativeVoice->SubmitSourceBuffer(&bufferInfo);
        }

        void PhoneSoundEmitter::setBufferEndCallback(void(*cb)(void *), void *userPtr) {
            _userCallback = cb;
            _userPointer = userPtr;
        }

        void PhoneSoundEmitter::setVolume(float volume) {
            _nativeVoice->SetVolume(volume);
        }

        void PhoneSoundEmitter::setWorldTransform(const math::m4x4 &matrix) {

        }

        void PhoneSoundEmitter::play() {
            _nativeVoice->Start();
        }

        void PhoneSoundEmitter::stop() {
            _nativeVoice->Stop();
        }

        void PhoneSoundEmitter::release() {
            if(_nativeVoice) {
                _nativeVoice->DestroyVoice();
            }

            _userCallback = nullptr;
            _nativeVoice = nullptr;

            delete this;
        }

        bool PhoneSoundEmitter::valid() const {
            return _nativeVoice != nullptr;
        }

        //---

        PhoneVertexBuffer::PhoneVertexBuffer(PhonePlatform *owner, platform::VertexType type, unsigned vcount, bool isDynamic, void *data) : PlatformObject(owner) {
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

        PhoneVertexBuffer::~PhoneVertexBuffer() {

        }

        void PhoneVertexBuffer::update(void *data) {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_self, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            if(mapres.pData) {
                memcpy(mapres.pData, data, _vcount * _vsize);
            }
            _owner->_context->Unmap(_self, 0);
        }

        void *PhoneVertexBuffer::lock() {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_self, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            return mapres.pData;
        }

        void PhoneVertexBuffer::unlock() {
            _owner->_context->Unmap(_self, 0);
        }

        void PhoneVertexBuffer::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        ID3D11Buffer *PhoneVertexBuffer::getBuffer() const {
            return _self;
        }

        unsigned PhoneVertexBuffer::getVertexCount() const {
            return _vcount;
        }

        unsigned PhoneVertexBuffer::getVertexSize() const {
            return _vsize;
        }

        bool PhoneVertexBuffer::valid() const {
            return _self != nullptr;
        }

        //--- 

        PhoneIndexedVertexBuffer::PhoneIndexedVertexBuffer(PhonePlatform *owner, platform::VertexType type, unsigned vcount, unsigned icount, bool isDynamic, void *vdata, void *idata) : PlatformObject(owner) {
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

        PhoneIndexedVertexBuffer::~PhoneIndexedVertexBuffer() {
        
        }

        void PhoneIndexedVertexBuffer::updateVertices(void *data) {
            D3D11_BOX tbox;
            tbox.back = 1;
            tbox.front = 0;
            tbox.left = 0;
            tbox.right = _vcount * _vsize;
            tbox.top = 0;
            tbox.bottom = 1;
            _owner->_context->UpdateSubresource(_vbuffer, 0, &tbox, data, 0, 0);
        }

        void PhoneIndexedVertexBuffer::updateIndices(void *data) {
            D3D11_BOX tbox;
            tbox.back = 1;
            tbox.front = 0;
            tbox.left = 0;
            tbox.right = _icount * sizeof(unsigned short);
            tbox.top = 0;
            tbox.bottom = 1;
            _owner->_context->UpdateSubresource(_ibuffer, 0, &tbox, data, 0, 0);
        }

        void *PhoneIndexedVertexBuffer::lockVertices() {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_vbuffer, 0, D3D11_MAP_WRITE, 0, &mapres);
            return mapres.pData;
        }

        void *PhoneIndexedVertexBuffer::lockIndices() {
            D3D11_MAPPED_SUBRESOURCE mapres = {0};
            _owner->_context->Map(_ibuffer, 0, D3D11_MAP_WRITE, 0, &mapres);
            return mapres.pData;
        }

        void PhoneIndexedVertexBuffer::unlockVertices() {
            _owner->_context->Unmap(_vbuffer, 0);
        }

        void PhoneIndexedVertexBuffer::unlockIndices() {
            _owner->_context->Unmap(_ibuffer, 0);
        }

        void PhoneIndexedVertexBuffer::release() {
            if(_vbuffer) {
                _vbuffer->Release();
            }
            if(_ibuffer) {
                _ibuffer->Release();
            }
            delete this;
        }

        ID3D11Buffer *PhoneIndexedVertexBuffer::getVBuffer() const {
            return _vbuffer;
        }

        ID3D11Buffer *PhoneIndexedVertexBuffer::getIBuffer() const {
            return _ibuffer;
        }

        unsigned PhoneIndexedVertexBuffer::getVertexCount() const {
            return _vcount;
        }

        unsigned PhoneIndexedVertexBuffer::getIndexCount() const {
            return _icount;
        }

        unsigned PhoneIndexedVertexBuffer::getVertexSize() const {
            return _vsize;
        }

        bool PhoneIndexedVertexBuffer::valid() const {
            return _vbuffer != nullptr && _ibuffer != nullptr;
        }


        //---

        PhoneRasterizerParams::PhoneRasterizerParams(PhonePlatform *owner, platform::CullMode cull) : PlatformObject(owner) {
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

        PhoneRasterizerParams::~PhoneRasterizerParams() {
        
        }

        void PhoneRasterizerParams::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void PhoneRasterizerParams::set() {
            _owner->_context->RSSetState(_self);
        }

        bool PhoneRasterizerParams::valid() const {
            return _self != nullptr;
        }

        //--- 

        PhoneBlenderParams::PhoneBlenderParams(PhonePlatform *owner, const platform::BlendMode blendMode) : PlatformObject(owner) {
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

        PhoneBlenderParams::~PhoneBlenderParams() {
        
        }

        void PhoneBlenderParams::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void PhoneBlenderParams::set() {
            _owner->_context->OMSetBlendState(_self, nullptr, 0xffffffff);
        }

        bool PhoneBlenderParams::valid() const {
            return _self != nullptr;
        }

        //--- 

        PhoneDepthParams::PhoneDepthParams(PhonePlatform *owner, bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) : PlatformObject(owner) {
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

        PhoneDepthParams::~PhoneDepthParams() {
        
        }

        void PhoneDepthParams::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void PhoneDepthParams::set() {
            _owner->_context->OMSetDepthStencilState(_self, 1);
        }

        bool PhoneDepthParams::valid() const {
            return _self != nullptr;
        }

        //--- 
        
        PhoneSampler::PhoneSampler(PhonePlatform *owner, platform::TextureFilter filter, platform::TextureAddressMode addrMode) : PlatformObject(owner) {
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

        PhoneSampler::~PhoneSampler() {
        
        }

        void PhoneSampler::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void PhoneSampler::set(platform::TextureSlot slot) {
            _owner->_context->PSSetSamplers(unsigned(slot), 1, &_self);
        }

        bool PhoneSampler::valid() const {
            return _self != nullptr;
        }

        //--- 

        PhoneShader::PhoneShader(PhonePlatform *owner, const byteform &binary) : PlatformObject(owner) {
            _vsh = nullptr;
            _psh = nullptr;
            _layout = nullptr;

            binary.readDword(); // received flags

            unsigned  offset = 0;
            unsigned  inputCount = binary.readDword();
            unsigned  vsLength = binary.readDword();
            unsigned  psLength = binary.readDword();
            char      inputNames[32][32];
            
            D3D11_INPUT_ELEMENT_DESC inputDesc[32] = {0};
        
            for(unsigned i = 0; i < inputCount; i++) {
                binary.readString(inputNames[i]);
                unsigned int floatCount = binary.readDword();

                inputDesc[i].SemanticName = inputNames[i];
                inputDesc[i].SemanticIndex = 0;
                inputDesc[i].Format = __nativeLayoutFormats[floatCount].format;
                inputDesc[i].InputSlot = 0;
                inputDesc[i].AlignedByteOffset = offset;
                inputDesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                inputDesc[i].InstanceDataStepRate = 0;

                offset += __nativeLayoutFormats[floatCount].size;
            }

            if(_owner->_device->CreateVertexShader(binary.getCurrentPtr(), vsLength, nullptr, &_vsh) == S_OK) {
                if(_owner->_device->CreateInputLayout(inputDesc, inputCount, binary.getCurrentPtr(), vsLength, &_layout) == S_OK) {
                    binary.incOffset(vsLength);
                    _owner->_device->CreatePixelShader(binary.getCurrentPtr(), psLength, nullptr, &_psh);
                }
            }
        }

        PhoneShader::~PhoneShader() {
        
        }

        void PhoneShader::release() {
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

        void PhoneShader::set() {
            _owner->_context->IASetInputLayout(_layout);
            _owner->_context->VSSetShader(_vsh, nullptr, 0);
            _owner->_context->PSSetShader(_psh, nullptr, 0);
        }

        bool PhoneShader::valid() const {
            return _psh != nullptr;
        }

        //--- 

        PhoneShaderConstantBuffer::PhoneShaderConstantBuffer(PhonePlatform *owner, platform::ShaderConstBufferUsing appoint, unsigned byteWidth) : PlatformObject(owner) {
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

        PhoneShaderConstantBuffer::~PhoneShaderConstantBuffer() {
        
        }

        void PhoneShaderConstantBuffer::update(const void *data) const {
            _owner->_context->UpdateSubresource(_self, 0, nullptr, data, 0, 0);
            
            //D3D11_MAPPED_SUBRESOURCE mapres = {0};
            //_owner->_context->Map(_self, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapres);
            //
            //if(mapres.pData) {
            //    memcpy(mapres.pData, data, _bytewidth);
            //}

            //_owner->_context->Unmap(_self, 0);
        }

        void PhoneShaderConstantBuffer::release() {
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        void PhoneShaderConstantBuffer::set() {
            _owner->_context->VSSetConstantBuffers(_inputIndex, 1, &_self);
            
            if(_inputIndex < unsigned(platform::ShaderConstBufferUsing::SKIN_DATA)) {
                _owner->_context->PSSetConstantBuffers(_inputIndex, 1, &_self);
            }
        }

        bool PhoneShaderConstantBuffer::valid() const {
            return _self != nullptr;
        }

        //---

        PhoneTexture2D::PhoneTexture2D() : PlatformObject(nullptr) {
            _self = nullptr;
            _view = nullptr;
            _width = 0;
            _height = 0;
            _mipCount = 0;
            _pixelsz = 0;
        }

        PhoneTexture2D::PhoneTexture2D(PhonePlatform *owner, unsigned char **imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) : PlatformObject(owner) {
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

        PhoneTexture2D::PhoneTexture2D(PhonePlatform *owner, platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount) : PlatformObject(owner) {
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

        PhoneTexture2D::~PhoneTexture2D() {

        }

        unsigned PhoneTexture2D::getWidth() const {
            return _width;
        }

        unsigned PhoneTexture2D::getHeight() const {
            return _height;
        }
        
        unsigned PhoneTexture2D::getMipCount() const {
            return _mipCount;
        }

        void PhoneTexture2D::update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) {
            D3D11_BOX tbox;
            tbox.back = 1;
            tbox.front = 0;
            tbox.left = x;
            tbox.right = x + w;
            tbox.top = y;
            tbox.bottom = y + h;
            _owner->_context->UpdateSubresource(_self, mip, &tbox, src, w * _pixelsz, 0);
        }

        void *PhoneTexture2D::getNativeHandle() const {
            return _self;
        }

        void PhoneTexture2D::release() {
            if(_view) {
                _view->Release();
            }
            if(_self) {
                _self->Release();
            }
            delete this;
        }

        bool PhoneTexture2D::valid() const {
            return _view != nullptr;
        }

        void PhoneTexture2D::set(platform::TextureSlot slot) {
            _owner->_context->PSSetShaderResources(unsigned(slot), 1, &_view);
        }

        //---

        PhoneRenderTarget::PhoneRenderTarget(PhonePlatform *owner) : PlatformObject(owner) {
            _depthView = nullptr;
            _depthTexture._owner = owner;
            
            for(unsigned i = 0; i < platform::RENDERTARGETS_MAX; i++) {
                _rtView[i] = nullptr;
                _renderTexture[i]._owner = owner;
            }

            _colorTargetCount = 0;
        }

        PhoneRenderTarget::PhoneRenderTarget(PhonePlatform *owner, unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) : PlatformObject(owner) {
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

        PhoneRenderTarget::~PhoneRenderTarget() {

        }

        platform::Texture2DInterface *PhoneRenderTarget::getDepthBuffer() {
            return &_depthTexture;
        }

        platform::Texture2DInterface *PhoneRenderTarget::getRenderBuffer(unsigned index) {
            return &_renderTexture[index]; 
        }

        void PhoneRenderTarget::release() {
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

        void PhoneRenderTarget::set() {            
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
        
        bool PhoneRenderTarget::valid() const {
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
        
        PhonePlatform::PhonePlatform(const diag::LogInterface &log) : _log(log), _defRenderTarget(this) {
            _device = nullptr;
            _context = nullptr;
            _swapChain = nullptr;
            _defSampler = nullptr;
            _nativeWidth = 0.0f;
            _nativeHeight = 0.0f;
            _syncInterval = 0;
            _curRenderTarget = nullptr;
        }
        
        bool PhonePlatform::init(const platform::InitParams &initParams) {
            PhoneInitParams &params = (PhoneInitParams &)initParams;
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

            unsigned flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
            //unsigned flags = 0; //D3D11_CREATE_DEVICE_DEBUG; //| D3D11_CREATE_DEVICE_BGRA_SUPPORT
            
            D3D_FEATURE_LEVEL features[] = {
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1
            };

            ID3D11DeviceContext  *tcontext;
            ID3D11Device         *tdevice;
            D3D_FEATURE_LEVEL    featureLevel;

            D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, features, 4, D3D11_SDK_VERSION, &tdevice, &featureLevel, &tcontext);

            tdevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&_device);
            tcontext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void **)&_context);
            tdevice->Release();
            tcontext->Release();

            _orientation = params.orientation;
            _nativeWidth = params.scrWidth;
            _nativeHeight = params.scrHeight;
            _syncInterval = params.syncInterval;
            _window = params.window;

            DXGI_MODE_ROTATION rotationMode = DXGI_MODE_ROTATION_IDENTITY;
            DisplayInformation ^curDisplayInfo = DisplayInformation::GetForCurrentView();

            if(params.orientation == platform::Orientation::ALBUM) {
                if(curDisplayInfo->NativeOrientation == DisplayOrientations::Portrait || curDisplayInfo->NativeOrientation == DisplayOrientations::PortraitFlipped) {
                    _nativeWidth = params.scrHeight;
                    _nativeHeight = params.scrWidth;
                    _inputTransform.setRotate(3.0f * M_PI / 2.0f);
                    _inputTransform._32 = _nativeHeight;
                    rotationMode = DXGI_MODE_ROTATION_ROTATE270;
                }
                if(curDisplayInfo->NativeOrientation == DisplayOrientations::LandscapeFlipped) {
                    rotationMode = DXGI_MODE_ROTATION_ROTATE90;
                }
            }
            else {
                if(curDisplayInfo->NativeOrientation == DisplayOrientations::Landscape || curDisplayInfo->NativeOrientation == DisplayOrientations::LandscapeFlipped) {
                    _nativeWidth = params.scrHeight;
                    _nativeHeight = params.scrWidth;
                    _inputTransform.setRotate(3.0f * M_PI / 2.0f);
                    _inputTransform._32 = _nativeHeight;
                    rotationMode = DXGI_MODE_ROTATION_ROTATE270;
                }
            }
            
            IDXGIAdapter  *adapter;
            IDXGIDevice1  *dxgiDevice = 0;
            IDXGIFactory2 *dxgiFactory;
            _device->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice);

            dxgiDevice->GetAdapter(&adapter);
            adapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory);
            adapter->Release();

            if(_window != nullptr) {
                DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
                swapChainDesc.Width = int(_nativeWidth);
                swapChainDesc.Height = int(_nativeHeight);
                swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_B8G8R8A8_UNORM;
                swapChainDesc.Stereo = false;
                swapChainDesc.SampleDesc.Count = 1;
                swapChainDesc.SampleDesc.Quality = 0;
                swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swapChainDesc.BufferCount = 2;
                swapChainDesc.Scaling = DXGI_SCALING_NONE;
                swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
                swapChainDesc.Flags = 0;

                if(dxgiFactory->CreateSwapChainForCoreWindow(_device, (IUnknown *)params.window.Get(), &swapChainDesc, nullptr, &_swapChain) != S_OK) {
                    _log.msgError("can't create hardware device");
                    return false;
                }

                _swapChain->SetRotation(rotationMode);
            }

            dxgiFactory->Release();
            dxgiDevice->SetMaximumFrameLatency(1);
            dxgiDevice->Release();

            D3D11_VIEWPORT vp;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;
            vp.Width = _nativeWidth;
            vp.Height = _nativeHeight;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            _context->RSSetViewports(1, &vp);

            //---

            ID3D11Texture2D *tmptex;
            _defRenderTarget._owner = this;

            if(_window == nullptr) {
                D3D11_TEXTURE2D_DESC tdesc = {0};
                tdesc.Width = int(_nativeWidth);
                tdesc.Height = int(_nativeHeight);
                tdesc.MipLevels = 1;
                tdesc.ArraySize = 1;
                tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM;
                tdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
                tdesc.Usage = D3D11_USAGE_DEFAULT;
                tdesc.SampleDesc.Count = 1;
                tdesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX | D3D11_RESOURCE_MISC_SHARED_NTHANDLE;

                _device->CreateTexture2D(&tdesc, nullptr, &_defRenderTarget._renderTexture->_self);
                _device->CreateRenderTargetView(_defRenderTarget._renderTexture->_self, nullptr, &_defRenderTarget._rtView[0]);
            }
            else {
                _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&tmptex);
                if(_device->CreateRenderTargetView(tmptex, 0, &_defRenderTarget._rtView[0]) != S_OK) {
                    _swapChain->Release();
                    _context->Release();
                    _device->Release();
                    _swapChain = nullptr;
                    _context = nullptr;
                    _device = nullptr;
                    _log.msgError("can't create default render target");
                    return false;
                }
                tmptex->Release();
            }

            DXGI_FORMAT depthTexFormat = DXGI_FORMAT_R24G8_TYPELESS; //DXGI_FORMAT_R32_TYPELESS; dx10
            DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT;
            UINT        depthTexBind = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

            D3D11_TEXTURE2D_DESC depthTexDesc = {0};
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

            D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {depthFormat};
            depthDesc.Format = depthFormat;
            depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depthDesc.Texture2D.MipSlice = 0;

            if(_device->CreateTexture2D(&depthTexDesc, 0, &tmptex) != S_OK) {
                _defRenderTarget._rtView[0]->Release();
                _defRenderTarget._rtView[0] = nullptr;
                _swapChain->Release();
                _context->Release();
                _device->Release();
                _swapChain = nullptr;
                _context = nullptr;
                _device = nullptr;
                _log.msgError("can't create zbuffer texture");
                return false;
            }
            _device->CreateDepthStencilView(tmptex, &depthDesc, &_defRenderTarget._depthView);

            D3D11_SHADER_RESOURCE_VIEW_DESC texViewDesc = {DXGI_FORMAT_R24_UNORM_X8_TYPELESS};
            texViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            texViewDesc.Texture2D.MipLevels = -1;
            texViewDesc.Texture2D.MostDetailedMip = 0;

            _device->CreateShaderResourceView(tmptex, &texViewDesc, &_defRenderTarget._depthTexture._view);
            tmptex->Release();

            _defSampler = new PhoneSampler (this, platform::TextureFilter::LINEAR, platform::TextureAddressMode::CLAMP);
            _defRenderTarget._depthTexture._width = unsigned(_nativeWidth);
            _defRenderTarget._depthTexture._height = unsigned(_nativeHeight);
            _defRenderTarget._depthTexture._mipCount = 1;
            _defRenderTarget._renderTexture[0]._width = unsigned(_nativeWidth);
            _defRenderTarget._renderTexture[0]._height = unsigned(_nativeHeight);
            _defRenderTarget._renderTexture[0]._mipCount = 1;
            _defRenderTarget._colorTargetCount = 1;
            _context->OMSetRenderTargets(1, &_defRenderTarget._rtView[0], _defRenderTarget._depthView);

            _curRenderTarget = &_defRenderTarget;

            for(unsigned i = 0; i < platform::TEXTURE_UNITS_MAX; i++) {
                _lastTextureWidth[i] = 0.0f;
                _lastTextureHeight[i] = 0.0f;
                rdSetSampler(platform::TextureSlot(i), _defSampler);
            }

            D3D11_RECT rect;
            rect.top = 0;
            rect.left = 0;
            rect.right = int(_nativeWidth) - 1;
            rect.bottom = int(_nativeHeight) - 1;

            _context->RSSetScissorRects(1, &rect);
            return true;
        }

        void PhonePlatform::destroy() {  
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
        
        float PhonePlatform::getScreenWidth() const {
            return _nativeWidth;
        }

        float PhonePlatform::getScreenHeight() const {
            return _nativeHeight;
        }

        float PhonePlatform::getCurrentRTWidth() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getWidth());
        }

        float PhonePlatform::getCurrentRTHeight() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getHeight());
        }

        float PhonePlatform::getTextureWidth(platform::TextureSlot slot) const {
            return _lastTextureWidth[unsigned(slot)];
        }

        float PhonePlatform::getTextureHeight(platform::TextureSlot slot) const {
            return _lastTextureHeight[unsigned(slot)];
        }

        const math::m3x3 &PhonePlatform::getInputTransform() const {
            return _inputTransform;
        }

        unsigned PhonePlatform::getMemoryUsing() const {
            return unsigned(Windows::System::MemoryManager::AppMemoryUsage / 1024);
        }

        unsigned PhonePlatform::getMemoryLimit() const {
            return unsigned(Windows::System::MemoryManager::AppMemoryUsageLimit / 1024);
        }

        unsigned  long long PhonePlatform::getTimeMs() const {
            unsigned __int64 ttime;
            GetSystemTimeAsFileTime((FILETIME *)&ttime);
            return ttime / 10000;
        }

        void PhonePlatform::updateOrientation() {
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
                    _inputTransform.setRotate(3.0f * M_PI / 2.0f);
                    _inputTransform._32 = _nativeHeight;
                    rotationMode = DXGI_MODE_ROTATION_ROTATE90;
                }
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::Landscape) {
                    _inputTransform.identity();                    
                    rotationMode = DXGI_MODE_ROTATION_ROTATE270;
                }
                if(curDisplayInfo->CurrentOrientation == DisplayOrientations::LandscapeFlipped) {
                    _inputTransform.identity();                    
                    rotationMode = DXGI_MODE_ROTATION_ROTATE90;
                }
            }
            else {
                _inputTransform.identity();
                rotationMode = DXGI_MODE_ROTATION_IDENTITY;                
            }

            _swapChain->SetRotation(rotationMode);
        }

        void PhonePlatform::fsFormFilesList(const char *path, std::string &out) {
            struct fn {
                static void formListW(const wchar_t *pathw, std::string &out) {
                    try {
                        Windows::Storage::StorageFolder ^fldr = create_task(Windows::ApplicationModel::Package::Current->InstalledLocation->GetFolderAsync(ref new Platform::String(pathw))).get();
                        Collections::IVectorView <StorageFile ^> ^files = create_task(fldr->GetFilesAsync()).get();
                        Collections::IVectorView <StorageFolder ^> ^folders = create_task(fldr->GetFoldersAsync()).get();

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

        bool PhonePlatform::fsLoadFile(const char *path, void **oBinaryDataPtr, unsigned int *oSize) {
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

        void PhonePlatform::sndSetGlobalVolume(float volume) {
            _mastering->SetVolume(volume);
        }

        platform::SoundEmitterInterface *PhonePlatform::sndCreateEmitter(unsigned sampleRate, unsigned channels) {
            PhoneSoundEmitter *r = new PhoneSoundEmitter (this, sampleRate, channels);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create sound emitter");
                delete r;
                return nullptr;
            }
        }

        platform::VertexBufferInterface *PhonePlatform::rdCreateVertexBuffer(platform::VertexType vtype, unsigned vcount, bool isDynamic, void *data) {
            PhoneVertexBuffer *r = new PhoneVertexBuffer (this, vtype, vcount, isDynamic, data);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create vertex buffer");
                delete r;
                return nullptr;
            }
        }

        platform::IndexedVertexBufferInterface *PhonePlatform::rdCreateIndexedVertexBuffer(platform::VertexType vtype, unsigned vcount, unsigned ushortIndexCount, bool isDynamic, void *vdata, void *idata) {
            PhoneIndexedVertexBuffer *r = new PhoneIndexedVertexBuffer (this, vtype, vcount, ushortIndexCount, isDynamic, vdata, idata);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create indexed vertex buffer");
                delete r;
                return nullptr;
            }
        }

        platform::ShaderInterface *PhonePlatform::rdCreateShader(const byteform &binary) {
            PhoneShader *r = new PhoneShader (this, binary);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create shader");
                delete r;
                return nullptr;
            }
        }
        
        platform::RasterizerParamsInterface *PhonePlatform::rdCreateRasterizerParams(platform::CullMode cull) {
            PhoneRasterizerParams *r = new PhoneRasterizerParams (this, cull);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create rasterizer state");
                delete r;
                return nullptr;
            }
        }

        platform::BlenderParamsInterface *PhonePlatform::rdCreateBlenderParams(const platform::BlendMode blendMode) {
            PhoneBlenderParams *r = new PhoneBlenderParams (this, blendMode);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create blender state");
                delete r;
                return nullptr;
            }
        }

        platform::DepthParamsInterface *PhonePlatform::rdCreateDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) {
            PhoneDepthParams *r = new PhoneDepthParams (this, depthEnabled, compareFunc, depthWriteEnabled);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create depth-stencil state");
                delete r;
                return nullptr;
            }
        }

        platform::SamplerInterface *PhonePlatform::rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode) {
            PhoneSampler *r = new PhoneSampler (this, filter, addrMode);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create sampler state");
                delete r;
                return nullptr;
            }
        }

        platform::ShaderConstantBufferInterface *PhonePlatform::rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) {
            PhoneShaderConstantBuffer *r = new PhoneShaderConstantBuffer (this, appoint, byteWidth);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("cant't create shader constant buffer");
                delete r;
                return nullptr;
            }
        }

        platform::Texture2DInterface *PhonePlatform::rdCreateTexture2D(unsigned char **imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            PhoneTexture2D *r = new PhoneTexture2D(this, imgMipsBinaryData, originWidth, originHeight, mipCount);

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

        platform::Texture2DInterface *PhonePlatform::rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            PhoneTexture2D *r = new PhoneTexture2D (this, format, originWidth, originHeight, mipCount);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("can't create texture2d with format");
                delete r;
                return nullptr;
            }
        }

        platform::RenderTargetInterface *PhonePlatform::rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) {
            PhoneRenderTarget *r = new PhoneRenderTarget (this, colorTargetCount, originWidth, originHeight);

            if(r->valid()) {
                return r;
            }
            else {
                _log.msgError("can't create render target");
                delete r;
                return nullptr;
            }
        }

        platform::RenderTargetInterface *PhonePlatform::rdGetDefaultRenderTarget() {
            return &_defRenderTarget;
        }

        void PhonePlatform::rdClearCurrentDepthBuffer(float depth) {
            _context->ClearDepthStencilView(_curRenderTarget->_depthView, D3D11_CLEAR_DEPTH, depth, 0);
        }

        void PhonePlatform::rdClearCurrentColorBuffer(const platform::color &c) {
            for(unsigned i = 0; i < _curRenderTarget->_colorTargetCount; i++) {
                _context->ClearRenderTargetView(_curRenderTarget->_rtView[i], (float *)&c);
            }
        }

        void PhonePlatform::rdSetRenderTarget(const platform::RenderTargetInterface *rt) {
            PhoneRenderTarget *dxObject = (PhoneRenderTarget *)rt;
            _curRenderTarget = dxObject;
            dxObject->set();            
        }

        void PhonePlatform::rdSetShader(const platform::ShaderInterface *shader) {
            PhoneShader *dxObject = (PhoneShader *)shader;
            dxObject->set();
        }

        void PhonePlatform::rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) {
            PhoneRasterizerParams *dxObj = (PhoneRasterizerParams *)params;
            dxObj->set();
        }

        void PhonePlatform::rdSetBlenderParams(const platform::BlenderParamsInterface *params) {
            PhoneBlenderParams *dxObj = (PhoneBlenderParams *)params;
            dxObj->set();
        }

        void PhonePlatform::rdSetDepthParams(const platform::DepthParamsInterface *params) {
            PhoneDepthParams *dxObj = (PhoneDepthParams *)params;
            dxObj->set();
        }

        void PhonePlatform::rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) {
            PhoneSampler *dxObj = (PhoneSampler *)sampler;
            dxObj->set(slot);
        }

        void PhonePlatform::rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) {
            PhoneShaderConstantBuffer *dxObj = (PhoneShaderConstantBuffer *)cbuffer;
            dxObj->set();
        }

        void PhonePlatform::rdSetTexture2D(platform::TextureSlot slot, const platform::Texture2DInterface *texture) {
            if(texture) {
                PhoneTexture2D *dxObj = (PhoneTexture2D *)texture;
                dxObj->set(slot);
                _lastTextureWidth[unsigned(slot)] = float(dxObj->getWidth());
                _lastTextureHeight[unsigned(slot)] = float(dxObj->getHeight());
            }
            else {
                ID3D11ShaderResourceView *tnull = nullptr;
                _context->PSSetShaderResources(unsigned(slot), 1, &tnull);
            }
        }

        void PhonePlatform::rdSetScissorRect(math::p2d &topLeft, math::p2d &bottomRight) {
            D3D11_RECT rect;
            rect.left = int(topLeft.x);
            rect.top = int(topLeft.y);
            rect.right = int(bottomRight.x);
            rect.bottom = int(bottomRight.y);
            _context->RSSetScissorRects(1, &rect);
        }

        void PhonePlatform::rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, platform::PrimitiveTopology topology, unsigned vertexCount) {
            PhoneVertexBuffer *dxObj = (PhoneVertexBuffer *)vbuffer;
            unsigned int offset = 0;
            unsigned int stride = dxObj->getVertexSize();
            ID3D11Buffer *buf = dxObj->getBuffer();

            _context->IASetVertexBuffers(0, 1, &buf, &stride, &offset);
            _context->IASetPrimitiveTopology(__nativeTopology[(unsigned int)topology]);
            _context->Draw(vertexCount, 0);
        }

        void PhonePlatform::rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, platform::PrimitiveTopology topology, unsigned indexCount) {
            PhoneIndexedVertexBuffer *dxObj = (PhoneIndexedVertexBuffer *)ivbuffer;
            unsigned int offset = 0;
            unsigned int stride = dxObj->getVertexSize();
            ID3D11Buffer *vbuf = dxObj->getVBuffer();
            ID3D11Buffer *ibuf = dxObj->getIBuffer();

            _context->IASetVertexBuffers(0, 1, &vbuf, &stride, &offset);
            _context->IASetIndexBuffer(ibuf, DXGI_FORMAT_R16_UINT, 0);
            _context->IASetPrimitiveTopology(__nativeTopology[(unsigned int)topology]);
            _context->DrawIndexed(indexCount, 0, 0);
        }

        void PhonePlatform::rdPresent() {
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

        bool PhonePlatform::isInited() {
            return _device != nullptr;
        }
    }
}




