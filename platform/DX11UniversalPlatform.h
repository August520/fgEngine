
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#include <xaudio2.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2.lib")

#include <ppltasks.h>
#include <agile.h>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;
using namespace concurrency;

namespace fg {
    namespace dx11 {
        class UniversalPlatform;
        class PlatformObject {
        public:
            PlatformObject(UniversalPlatform *owner) : _owner(owner) {}
            virtual ~PlatformObject() {}
            virtual bool valid() const = 0;

        protected:
            UniversalPlatform *_owner;

        private:
            PlatformObject(const PlatformObject &);
            PlatformObject &operator =(const PlatformObject &);
        };

        //---

        class UniversalSoundEmitter : public PlatformObject, public platform::SoundEmitterInterface {
        public:
            UniversalSoundEmitter(UniversalPlatform *owner, unsigned sampleRate, unsigned channels);
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

            void (*_userCallback)(void *);
            void *_userPointer;

            IXAudio2SourceVoice  *_nativeVoice;
            SoundCallback        _sndCallback;
            unsigned             _channels;
        };

        //---

        class UniversalVertexBuffer : public PlatformObject, public platform::VertexBufferInterface {
        public:
            UniversalVertexBuffer(UniversalPlatform *owner, platform::VertexType type, unsigned vcount, bool isDynamic, const void *data);
            ~UniversalVertexBuffer() override;

            void *lock() override;
            void unlock() override;
            void release() override;

            ID3D11Buffer   *getBuffer() const;
            unsigned       getVertexCount() const;
            unsigned       getVertexSize() const;

            bool valid() const override;
            
        protected:
            ID3D11Buffer   *_self;
            unsigned       _vcount;
            unsigned       _vsize;
        };

        //--- 

        class UniversalIndexedVertexBuffer : public PlatformObject, public platform::IndexedVertexBufferInterface {
        public:
            UniversalIndexedVertexBuffer(UniversalPlatform *owner, platform::VertexType type, unsigned vcount, unsigned icount, bool isDynamic, const void *vdata, const void *idata);
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
            ID3D11Buffer   *_vbuffer;
            ID3D11Buffer   *_ibuffer;
            unsigned       _vcount;
            unsigned       _vsize;
            unsigned       _icount;
        };

        //---

        class UniversalInstanceData : public PlatformObject, public platform::InstanceDataInterface {
        public:
            UniversalInstanceData(UniversalPlatform *owner, platform::InstanceDataType type, unsigned instanceCount);
            ~UniversalInstanceData() override;

            void *lock() override;
            void unlock() override;
            void update(const void *data, unsigned instanceCount) override;

            void release() override;
            bool valid() const override;

            ID3D11Buffer   *getBuffer() const;
            unsigned       getInstanceDataSize() const;

        protected:
            ID3D11Buffer   *_instanceBuffer;
            unsigned       _instanceCount;
            unsigned       _instanceDataSize;
        };

        //--- 

        class UniversalRasterizerParams : public PlatformObject, public platform::RasterizerParamsInterface {
        public:
            UniversalRasterizerParams(UniversalPlatform *owner, platform::CullMode cull);
            ~UniversalRasterizerParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11RasterizerState *_self;
        };

        //--- 

        class UniversalBlenderParams : public PlatformObject, public platform::BlenderParamsInterface {
        public:
            UniversalBlenderParams(UniversalPlatform *owner, const platform::BlendMode blendMode);
            ~UniversalBlenderParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11BlendState *_self;
        };

        //--- 

        class UniversalDepthParams : public PlatformObject, public platform::DepthParamsInterface {
        public:
            UniversalDepthParams(UniversalPlatform *owner, bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled);
            ~UniversalDepthParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11DepthStencilState *_self;
        };

        //--- 

        class UniversalSampler : public PlatformObject, public platform::SamplerInterface {
        public:
            UniversalSampler(UniversalPlatform *owner, platform::TextureFilter filter, platform::TextureAddressMode addrMode);
            ~UniversalSampler() override;

            void release() override;
            void set(platform::TextureSlot slot);
            bool valid() const override;

        protected:
            ID3D11SamplerState *_self;
        };

        //--- 

        class UniversalShader : public PlatformObject, public platform::ShaderInterface {
        public:
            UniversalShader(UniversalPlatform *owner, const byteform &binary);
            ~UniversalShader() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11VertexShader  *_vsh;
            ID3D11PixelShader   *_psh;
            ID3D11InputLayout   *_layout;
        };
        
        //---

        class UniversalShaderConstantBuffer : public PlatformObject, public platform::ShaderConstantBufferInterface {
        public:
            UniversalShaderConstantBuffer(UniversalPlatform *owner, platform::ShaderConstBufferUsing appoint, unsigned byteWidth);
            ~UniversalShaderConstantBuffer() override;

            void update(const void *data, unsigned byteWidth) override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11Buffer   *_self;
            unsigned       _inputIndex;
            unsigned       _bytewidth;
        };

        //--- 

        class UniversalTexture2D : public PlatformObject, public platform::Texture2DInterface {
            friend class UniversalRenderTarget;
            friend class UniversalPlatform;

        public:
            UniversalTexture2D();
            UniversalTexture2D(UniversalPlatform *owner, unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            UniversalTexture2D(UniversalPlatform *owner, platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            ~UniversalTexture2D() override;

            unsigned getWidth() const override;
            unsigned getHeight() const override;
            unsigned getMipCount() const override;

            void  update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) override;
            
            void  *getNativeHandle() const override;
            void  release() override;
            bool  valid() const override;
            void  set(platform::TextureSlot slot);

        protected:
            unsigned  _width;
            unsigned  _height;
            unsigned  _mipCount;
            unsigned  _pixelsz;
            
            ID3D11Texture2D           *_self;
            ID3D11ShaderResourceView  *_view;
        };

        //---

        class UniversalRenderTarget : public PlatformObject, public platform::RenderTargetInterface {
            friend class UniversalPlatform;

        public:
            UniversalRenderTarget(UniversalPlatform *owner);
            UniversalRenderTarget(UniversalPlatform *owner, unsigned colorTargetCount, unsigned originWidth, unsigned originHeight);
            ~UniversalRenderTarget() override;

            platform::Texture2DInterface *getDepthBuffer() override;
            platform::Texture2DInterface *getRenderBuffer(unsigned index) override;

            void  release() override;
            bool  valid() const override;
            void  set();

        protected:
            unsigned _colorTargetCount;

            UniversalTexture2D  _renderTexture[platform::RENDERTARGETS_MAX];
            UniversalTexture2D  _depthTexture;

            ID3D11RenderTargetView  *_rtView[platform::RENDERTARGETS_MAX];
            ID3D11DepthStencilView  *_depthView;
        };

        //---

        struct UniversalInitParams : public platform::InitParams {
            platform::Orientation   orientation;
            Agile <CoreWindow ^>    window;           // may be null
        };

        class UniversalPlatform : public platform::EnginePlatformInterface {
        public:
            ID3D11Device1         *_device = nullptr;
            ID3D11DeviceContext1  *_context = nullptr;
            IDXGISwapChain1       *_swapChain = nullptr;
            IXAudio2              *_audio = nullptr;

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
            platform::ShaderInterface                *rdCreateShader(const byteform &binary) override;
            platform::RasterizerParamsInterface      *rdCreateRasterizerParams(platform::CullMode cull) override;
            platform::BlenderParamsInterface         *rdCreateBlenderParams(const platform::BlendMode blendMode) override; 
            platform::DepthParamsInterface           *rdCreateDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) override; 
            platform::SamplerInterface               *rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode) override; //!
            platform::ShaderConstantBufferInterface  *rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) override;
            platform::Texture2DInterface             *rdCreateTexture2D(unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::Texture2DInterface             *rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::RenderTargetInterface          *rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) override;
            platform::RenderTargetInterface          *rdGetDefaultRenderTarget() override;

            void  rdClearCurrentDepthBuffer(float depth = 1.0f) override;
            void  rdClearCurrentColorBuffer(const fg::color &c = fg::color(0.0f, 0.0f, 0.0f, 0.0f)) override;

            void  rdSetRenderTarget(const platform::RenderTargetInterface *rt) override;
            void  rdSetShader(const platform::ShaderInterface *shader) override;
            void  rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) override;
            void  rdSetBlenderParams(const platform::BlenderParamsInterface *params) override;
            void  rdSetDepthParams(const platform::DepthParamsInterface *params) override;
            void  rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) override;
            void  rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) override;
            void  rdSetTexture2D(platform::TextureSlot slot, const platform::Texture2DInterface *texture) override;
            void  rdSetScissorRect(math::p2d &topLeft, math::p2d &bottomRight) override;
            
            void  rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned vertexCount, unsigned instanceCount) override;
            void  rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned indexCount, unsigned instanceCount) override;
            void  rdPresent() override;

            bool  isInited() const override;

        protected:
            const diag::LogInterface &_log;

            IXAudio2MasteringVoice   *_mastering = nullptr;
            UniversalRenderTarget    *_curRenderTarget = nullptr;
            UniversalSampler         *_defSampler = nullptr;
            UniversalRenderTarget    _defRenderTarget = nullptr;
            Agile                    <CoreWindow ^> _window;
            platform::Orientation    _orientation = platform::Orientation::ALBUM;
            math::m3x3               _inputTransform;
            
            float      _nativeWidth;
            float      _nativeHeight;
            float      _lastTextureWidth[platform::TEXTURE_UNITS_MAX];
            float      _lastTextureHeight[platform::TEXTURE_UNITS_MAX];
            unsigned   _syncInterval;

            void _initDevice();
            void _initDefaultRenderTarget();
        };
    }
}

