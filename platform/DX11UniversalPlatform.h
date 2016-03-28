
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#include <xaudio2.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2.lib")

#include <dxgi1_3.h>
#include <windows.ui.xaml.media.dxinterop.h>

#include <ppltasks.h>
#include <agile.h>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace concurrency;

namespace fg {
    namespace dx11 {
        class UniversalPlatform;
        class PlatformObject {
            friend class UniversalPlatform;

        public:
            virtual ~PlatformObject() {}
            virtual bool valid() const = 0;

        protected:
            PlatformObject(UniversalPlatform *owner) : _owner(owner) {}
            
            UniversalPlatform *_owner;

        private:
            PlatformObject(const PlatformObject &) {}
            PlatformObject &operator =(const PlatformObject &) {}
        };

        //---

        class UniversalSoundEmitter : public PlatformObject, public platform::SoundEmitterInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalSoundEmitter() override;

            void pushBuffer(const char *data, unsigned samples) override;
            void setBufferEndCallback(void (*cb)(void *), void *userPtr) override;
            void setVolume(float volume) override;
            void setWorldTransform(const math::m4x4 &matrix) override;
            void play() override;
            void stop() override;
            void release() override;

            bool valid() const override;

        protected:
            UniversalSoundEmitter(UniversalPlatform *owner, unsigned sampleRate, unsigned channels);
            
            struct SoundCallback : public IXAudio2VoiceCallback {
                UniversalSoundEmitter *emitter;
                SoundCallback(UniversalSoundEmitter *iemitter) : emitter(iemitter) {}

                void __stdcall OnVoiceProcessingPassStart(UINT32 BytesRequired) {}
                void __stdcall OnVoiceProcessingPassEnd() {}
                void __stdcall OnStreamEnd() {}
                void __stdcall OnBufferStart(void *pBufferContext) {}
                void __stdcall OnBufferEnd(void *pBufferContext);
                void __stdcall OnLoopEnd(void *pBufferContext) {}
                void __stdcall OnVoiceError(void *pBufferContext, HRESULT Error) {}
            };

            void (*_userCallback)(void *) = nullptr;
            void *_userPointer = nullptr;

            IXAudio2SourceVoice  *_nativeVoice = nullptr;
            SoundCallback        _sndCallback;
            unsigned             _channels;
        };

        //---

        class UniversalVertexBuffer : public PlatformObject, public platform::VertexBufferInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalVertexBuffer() override;

            void *lock() override;
            void unlock() override;
            void release() override;

            ID3D11Buffer   *getBuffer() const;
            unsigned       getVertexCount() const;
            unsigned       getVertexSize() const;

            bool valid() const override;
            
        protected:
            UniversalVertexBuffer(UniversalPlatform *owner, platform::VertexType type, unsigned vcount, bool isDynamic, const void *data);
            
            ID3D11Buffer   *_self = nullptr;
            unsigned       _vcount;
            unsigned       _vsize;
        };

        //--- 

        class UniversalIndexedVertexBuffer : public PlatformObject, public platform::IndexedVertexBufferInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalIndexedVertexBuffer() override;

            void *lockVertices() override;
            void *lockIndices() override;
            void unlockVertices() override;
            void unlockIndices() override;
            void release() override;

            bool valid() const override;

            ID3D11Buffer   *getVBuffer() const;
            ID3D11Buffer   *getIBuffer() const;
            unsigned       getVertexCount() const;
            unsigned       getIndexCount() const;
            unsigned       getVertexSize() const;

        protected:
            UniversalIndexedVertexBuffer(UniversalPlatform *owner, platform::VertexType type, unsigned vcount, unsigned icount, bool isDynamic, const void *vdata, const void *idata);

            ID3D11Buffer   *_vbuffer = nullptr;
            ID3D11Buffer   *_ibuffer = nullptr;
            unsigned       _vcount;
            unsigned       _vsize;
            unsigned       _icount;
        };

        //---

        class UniversalInstanceData : public PlatformObject, public platform::InstanceDataInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalInstanceData() override;

            void *lock() override;
            void unlock() override;
            void update(const void *data, unsigned instanceCount) override;

            void release() override;
            bool valid() const override;

            ID3D11Buffer   *getBuffer() const;
            unsigned       getInstanceDataSize() const;

        protected:
            UniversalInstanceData(UniversalPlatform *owner, platform::InstanceDataType type, unsigned instanceCount);

            ID3D11Buffer   *_instanceBuffer = nullptr;
            unsigned       _instanceCount;
            unsigned       _instanceDataSize;
        };

        //--- 

        class UniversalRasterizerParams : public PlatformObject, public platform::RasterizerParamsInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalRasterizerParams() override;

            void release() override;
            void set() const;
            bool valid() const override;

        protected:
            UniversalRasterizerParams(UniversalPlatform *owner, platform::CullMode cull);

            ID3D11RasterizerState *_self = nullptr;
        };

        //--- 

        class UniversalBlenderParams : public PlatformObject, public platform::BlenderParamsInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalBlenderParams() override;

            void release() override;
            void set() const;
            bool valid() const override;

        protected:
            UniversalBlenderParams(UniversalPlatform *owner, const platform::BlendMode blendMode);

            ID3D11BlendState *_self = nullptr;
        };

        //--- 

        class UniversalDepthParams : public PlatformObject, public platform::DepthParamsInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalDepthParams() override;

            void release() override;
            void set() const;
            bool valid() const override;

        protected:
            UniversalDepthParams(UniversalPlatform *owner, bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled);

            ID3D11DepthStencilState *_self = nullptr;
        };

        //--- 

        class UniversalSampler : public PlatformObject, public platform::SamplerInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalSampler() override;

            void release() override;
            void set(platform::TextureSlot slot) const;
            bool valid() const override;

        protected:
            UniversalSampler(UniversalPlatform *owner, platform::TextureFilter filter, platform::TextureAddressMode addrMode, float minLod, float bias);

            ID3D11SamplerState *_self = nullptr;
        };

        //--- 

        class UniversalShader : public PlatformObject, public platform::ShaderInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalShader() override;

            void release() override;
            void set() const;
            bool valid() const override;

        protected:
            UniversalShader(UniversalPlatform *owner, const byteinput &binary);

            ID3D11VertexShader  *_vsh = nullptr;
            ID3D11PixelShader   *_psh = nullptr;
            ID3D11InputLayout   *_layout = nullptr;
        };
        
        //---

        class UniversalShaderConstantBuffer : public PlatformObject, public platform::ShaderConstantBufferInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalShaderConstantBuffer() override;

            void update(const void *data, unsigned byteWidth) override;
            void release() override;
            void set() const;
            bool valid() const override;

        protected:
            UniversalShaderConstantBuffer(UniversalPlatform *owner, platform::ShaderConstBufferUsing appoint, unsigned byteWidth);

            ID3D11Buffer   *_self = nullptr;
            unsigned       _inputIndex;
            unsigned       _bytewidth;
        };

        //--- 

        class UniversalTexture2D : public PlatformObject, public platform::Texture2DInterface {
            friend class UniversalRenderTarget;
            friend class UniversalCubeRenderTarget;
            friend class UniversalPlatform;

        public:
            ~UniversalTexture2D() override;

            unsigned getWidth() const override;
            unsigned getHeight() const override;
            unsigned getMipCount() const override;

            void  update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) override;
            void  release() override;
            bool  valid() const override;
            void  set(platform::TextureSlot slot) const;

        protected:
            UniversalTexture2D();
            UniversalTexture2D(UniversalPlatform *owner, unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount, platform::TextureFormat format);
            UniversalTexture2D(UniversalPlatform *owner, platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount);

            unsigned  _width = 0;
            unsigned  _height = 0;
            unsigned  _mipCount = 0;

            platform::TextureFormat   _format = platform::TextureFormat::UNKNOWN;
            ID3D11Texture2D           *_self = nullptr;
            ID3D11ShaderResourceView  *_view = nullptr;
        };

        //---

        class UniversalTextureCube : public PlatformObject, public platform::TextureCubeInterface {
            friend class UniversalCubeRenderTarget;
            friend class UniversalPlatform;

        public:
            ~UniversalTextureCube() override;

            void  release() override;
            bool  valid() const override;
            void  set(platform::TextureSlot slot) const;

        protected:
            UniversalTextureCube();
            UniversalTextureCube(UniversalPlatform *owner, unsigned char **imgMipsBinaryData[6], unsigned originSize, unsigned mipCount, platform::TextureFormat format);

            platform::TextureFormat   _format = platform::TextureFormat::UNKNOWN;
            ID3D11Texture2D           *_self = nullptr;
            ID3D11ShaderResourceView  *_view = nullptr;
        };

        //---

        class UniversalRenderTarget : public PlatformObject, public platform::RenderTargetInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalRenderTarget() override;

            const platform::Texture2DInterface *getDepthBuffer() const override;
            const platform::Texture2DInterface *getRenderBuffer(unsigned index) const override;
            
            unsigned getRenderBufferCount() const override;
            unsigned getWidth() const override;
            unsigned getHeight() const override;

            void  release() override;
            bool  valid() const override;

        protected:
            UniversalRenderTarget(UniversalPlatform *owner);
            UniversalRenderTarget(UniversalPlatform *owner, unsigned colorTargetCount, unsigned originWidth, unsigned originHeight, platform::RenderTargetType type);

            platform::RenderTargetType _type = platform::RenderTargetType::Normal;
            unsigned _colorTargetCount = 0;
            unsigned _width  = 0;
            unsigned _height = 0;

            UniversalTexture2D  _renderTextures[FG_RENDERTARGETS_MAX];
            UniversalTexture2D  _depthTexture;

            ID3D11RenderTargetView  *_rtViews[FG_RENDERTARGETS_MAX];
            ID3D11DepthStencilView  *_depthView = nullptr;
        };

        //---

        class UniversalCubeRenderTarget : public PlatformObject, public platform::CubeRenderTargetInterface {
            friend class UniversalPlatform;

        public:
            ~UniversalCubeRenderTarget() override;

            const platform::Texture2DInterface   *getDepthBuffer() const override;
            const platform::TextureCubeInterface *getRenderBuffer() const override;
            unsigned getSize() const override;
            
            void  release() override;
            bool  valid() const override;

        protected:
            UniversalCubeRenderTarget(UniversalPlatform *owner, unsigned originSize, platform::RenderTargetType type);

            platform::RenderTargetType _type = platform::RenderTargetType::Normal;
            unsigned _size = 0;

            UniversalTextureCube  _renderCube;
            UniversalTexture2D    _depthTexture;

            ID3D11RenderTargetView  *_rtViews[6];
            ID3D11DepthStencilView  *_depthView = nullptr;
        };

        //---

        struct UniversalInitParams : public platform::InitParams {
            platform::Orientation     orientation;
            Agile <CoreWindow ^>      window;           // may be null
            Agile <SwapChainPanel ^>  swapChainPanel;
        };

        class UniversalPlatform : public platform::EnginePlatformInterface {
        public:
            ID3D11Device1         *device = nullptr;
            ID3D11DeviceContext1  *context = nullptr;
            IDXGISwapChain1       *swapChain = nullptr;
            IXAudio2              *audio = nullptr;

            UniversalPlatform(const diag::LogInterface &log);

            bool  init(const platform::InitParams &initParams) override;
            void  destroy() override;
            
            float getScreenWidth() const override;
            float getScreenHeight() const override;
            float getCurrentRTWidth() const override;
            float getCurrentRTHeight() const override;

            float getTextureWidth(platform::TextureSlot slot) const override;
            float getTextureHeight(platform::TextureSlot slot) const override;

            const math::m3x3  &getInputTransform() const override;

            unsigned  getMemoryUsing() const override;
            unsigned  getMemoryLimit() const override;
            unsigned  long long getTimeMs() const override;
            
            void  updateOrientation() override;
            void  resize(float width, float height) override;
                        
            void  fsFormFilesList(const char *path, std::string &out) override;
            bool  fsLoadFile(const char *path, void **oBinaryDataPtr, unsigned int *oSize) override;
            bool  fsSaveFile(const char *path, void *iBinaryDataPtr, unsigned iSize) override;

            void  sndSetGlobalVolume(float volume) override;

            platform::SoundEmitterInterface          *sndCreateEmitter(unsigned sampleRate, unsigned channels) override;
            platform::VertexBufferInterface          *rdCreateVertexBuffer(platform::VertexType vtype, unsigned vcount, bool isDynamic, const void *data) override;
            platform::IndexedVertexBufferInterface   *rdCreateIndexedVertexBuffer(platform::VertexType vtype, unsigned vcount, unsigned ushortIndexCount, bool isDynamic, const void *vdata, const void *idata) override;
            platform::InstanceDataInterface          *rdCreateInstanceData(platform::InstanceDataType type, unsigned instanceCount) override;
            platform::ShaderInterface                *rdCreateShader(const byteinput &binary) override;
            platform::RasterizerParamsInterface      *rdCreateRasterizerParams(platform::CullMode cull) override;
            platform::BlenderParamsInterface         *rdCreateBlenderParams(const platform::BlendMode blendMode) override; 
            platform::DepthParamsInterface           *rdCreateDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) override; 
            platform::SamplerInterface               *rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode, float minLod, float bias) override; 
            platform::ShaderConstantBufferInterface  *rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) override;
            platform::Texture2DInterface             *rdCreateTexture2D(unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount, platform::TextureFormat fmt) override;
            platform::Texture2DInterface             *rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::TextureCubeInterface           *rdCreateTextureCube(unsigned char **imgMipsBinaryData[6], unsigned originSize, unsigned mipCount, platform::TextureFormat fmt) override;
            platform::RenderTargetInterface          *rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight, platform::RenderTargetType type) override;
            platform::CubeRenderTargetInterface      *rdCreateCubeRenderTarget(unsigned originSize, platform::RenderTargetType type) override;
            platform::RenderTargetInterface          *rdGetDefaultRenderTarget() override;

            void  rdClearCurrentDepthBuffer(float depth = 1.0f) override;
            void  rdClearCurrentColorBuffer(const fg::color &c = fg::color(0.0f, 0.0f, 0.0f, 0.0f)) override;

            void  rdSetRenderTarget(const platform::RenderTargetInterface *rt) override;
            void  rdSetCubeRenderTarget(const platform::CubeRenderTargetInterface *rt, unsigned faceIndex) override;
            void  rdSetShader(const platform::ShaderInterface *shader) override;
            void  rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) override;
            void  rdSetBlenderParams(const platform::BlenderParamsInterface *params) override;
            void  rdSetDepthParams(const platform::DepthParamsInterface *params) override;
            void  rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) override;
            void  rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) override;
            void  rdSetTexture2D(platform::TextureSlot slot, const platform::Texture2DInterface *texture) override;
            void  rdSetTextureCube(platform::TextureSlot slot, const platform::TextureCubeInterface *texture) override;
            void  rdSetScissorRect(const math::p2d &topLeft, const math::p2d &bottomRight) override;
            
            void  rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned vertexCount, unsigned instanceCount) override;
            void  rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned indexCount, unsigned instanceCount) override;
            void  rdPresent() override;

            bool  isInited() const override;

        protected:
            const diag::LogInterface   &_log;
            IXAudio2MasteringVoice     *_mastering = nullptr;
            UniversalSampler           *_defSampler = nullptr;
            UniversalRenderTarget      _defRenderTarget = nullptr;
            Agile                      <CoreWindow ^> _window = nullptr;
            Agile                      <SwapChainPanel ^> _swapChainPanel = nullptr;
            platform::Orientation      _orientation = platform::Orientation::ALBUM;
            math::m3x3                 _inputTransform;

            unsigned int _curRTWidth = 1;
            unsigned int _curRTHeight = 1;
            unsigned int _curRTColorTargetCount = 1;
            ID3D11RenderTargetView *_curRTColorViews[FG_RENDERTARGETS_MAX];
            ID3D11DepthStencilView *_curRTDepthView;

            float      _nativeWidth = 1.0f;
            float      _nativeHeight = 1.0f;
            float      _lastTextureWidth[FG_TEXTURE_UNITS_MAX];
            float      _lastTextureHeight[FG_TEXTURE_UNITS_MAX];
            unsigned   _syncInterval = 0;

            void _initDevice();
            void _initDefaultRenderTarget();
        };
    }
}

