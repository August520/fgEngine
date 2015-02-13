
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
        class PhonePlatform;
        class PlatformObject {
        public:
            PlatformObject(PhonePlatform *owner) : _owner(owner) {}
            virtual ~PlatformObject() {}
            virtual bool valid() const = 0;

        protected:
            PhonePlatform *_owner;

        private:
            PlatformObject(const PlatformObject &);
            PlatformObject &operator =(const PlatformObject &);
        };

        //---

        class PhoneSoundEmitter : public PlatformObject, public platform::SoundEmitterInterface {
        public:
            PhoneSoundEmitter(PhonePlatform *owner, unsigned sampleRate, unsigned channels);
            ~PhoneSoundEmitter() override;

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
                PhoneSoundEmitter *emitter;
                SoundCallback(PhoneSoundEmitter *iemitter) : emitter(iemitter) {}

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

        class PhoneVertexBuffer : public PlatformObject, public platform::VertexBufferInterface {
        public:
            PhoneVertexBuffer(PhonePlatform *owner, platform::VertexType type, unsigned vcount, bool isDynamic, void *data);
            ~PhoneVertexBuffer() override;

            void update(void *data) override;
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

        class PhoneIndexedVertexBuffer : public PlatformObject, public platform::IndexedVertexBufferInterface {
        public:
            PhoneIndexedVertexBuffer(PhonePlatform *owner, platform::VertexType type, unsigned vcount, unsigned icount, bool isDynamic, void *vdata, void *idata);
            ~PhoneIndexedVertexBuffer() override;

            void updateVertices(void *data) override;
            void updateIndices(void *data) override;

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

        class PhoneRasterizerParams : public PlatformObject, public platform::RasterizerParamsInterface {
        public:
            PhoneRasterizerParams(PhonePlatform *owner, platform::CullMode cull);
            ~PhoneRasterizerParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11RasterizerState *_self;
        };

        //--- 

        class PhoneBlenderParams : public PlatformObject, public platform::BlenderParamsInterface {
        public:
            PhoneBlenderParams(PhonePlatform *owner, const platform::BlendMode blendMode);
            ~PhoneBlenderParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11BlendState *_self;
        };

        //--- 

        class PhoneDepthParams : public PlatformObject, public platform::DepthParamsInterface {
        public:
            PhoneDepthParams(PhonePlatform *owner, bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled);
            ~PhoneDepthParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11DepthStencilState *_self;
        };

        //--- 

        class PhoneSampler : public PlatformObject, public platform::SamplerInterface {
        public:
            PhoneSampler(PhonePlatform *owner, platform::TextureFilter filter, platform::TextureAddressMode addrMode);
            ~PhoneSampler() override;

            void release() override;
            void set(platform::TextureSlot slot);
            bool valid() const override;

        protected:
            ID3D11SamplerState *_self;
        };

        //--- 

        class PhoneShader : public PlatformObject, public platform::ShaderInterface {
        public:
            PhoneShader(PhonePlatform *owner, const byteform &binary);
            ~PhoneShader() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11VertexShader  *_vsh;
            ID3D11PixelShader   *_psh;
            ID3D11InputLayout   *_layout;
        };
        
        //---

        class PhoneShaderConstantBuffer : public PlatformObject, public platform::ShaderConstantBufferInterface {
        public:
            PhoneShaderConstantBuffer(PhonePlatform *owner, platform::ShaderConstBufferUsing appoint, unsigned byteWidth);
            ~PhoneShaderConstantBuffer() override;

            void update(const void *data) const override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11Buffer   *_self;
            unsigned       _inputIndex;
            unsigned       _bytewidth;
        };

        //--- 

        class PhoneTexture2D : public PlatformObject, public platform::Texture2DInterface {
            friend class PhoneRenderTarget;
            friend class PhonePlatform;

        public:
            PhoneTexture2D();
            PhoneTexture2D(PhonePlatform *owner, unsigned char **imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            PhoneTexture2D(PhonePlatform *owner, platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            ~PhoneTexture2D() override;

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

        class PhoneRenderTarget : public PlatformObject, public platform::RenderTargetInterface {
            friend class PhonePlatform;

        public:
            PhoneRenderTarget(PhonePlatform *owner);
            PhoneRenderTarget(PhonePlatform *owner, unsigned colorTargetCount, unsigned originWidth, unsigned originHeight);
            ~PhoneRenderTarget() override;

            platform::Texture2DInterface *getDepthBuffer() override;
            platform::Texture2DInterface *getRenderBuffer(unsigned index) override;

            void  release() override;
            bool  valid() const override;
            void  set();

        protected:
            unsigned _colorTargetCount;

            PhoneTexture2D  _renderTexture[platform::RENDERTARGETS_MAX];
            PhoneTexture2D  _depthTexture;

            ID3D11RenderTargetView  *_rtView[platform::RENDERTARGETS_MAX];
            ID3D11DepthStencilView  *_depthView;
        };

        //---

        struct PhoneInitParams : public platform::InitParams {
            platform::Orientation   orientation;
            Agile <CoreWindow ^>    window;           // may be null
        };

        class PhonePlatform : public platform::EnginePlatformInterface {
        public:
            ID3D11Device1         *_device;
            ID3D11DeviceContext1  *_context;
            IDXGISwapChain1       *_swapChain;
            IXAudio2              *_audio;

            PhonePlatform(const diag::LogInterface &log);

            bool  init(const platform::InitParams &initParams) override;
            void  destroy() override;
            
            float getScreenWidth() const override;
            float getScreenHeight() const override;
            float getCurrentRTWidth() const override;
            float getCurrentRTHeight() const override;

            float getTextureWidth(platform::TextureSlot slot) const override;
            float getTextureHeight(platform::TextureSlot slot) const override;

            unsigned  getMemoryUsing() const override;
            unsigned  getMemoryLimit() const override;
            unsigned  long long getTimeMs() const override;
            
            void  updateOrientation() override;
            
            void  fsFormFilesList(const char *path, std::string &out) override;
            bool  fsLoadFile(const char *path, void **oBinaryDataPtr, unsigned int *oSize) override;
            void  sndSetGlobalVolume(float volume) override;

            platform::SoundEmitterInterface          *sndCreateEmitter(unsigned sampleRate, unsigned channels) override;
            platform::VertexBufferInterface          *rdCreateVertexBuffer(platform::VertexType vtype, unsigned vcount, bool isDynamic, void *data) override;
            platform::IndexedVertexBufferInterface   *rdCreateIndexedVertexBuffer(platform::VertexType vtype, unsigned vcount, unsigned ushortIndexCount, bool isDynamic, void *vdata, void *idata) override;
            platform::ShaderInterface                *rdCreateShader(const byteform &binary) override;
            platform::RasterizerParamsInterface      *rdCreateRasterizerParams(platform::CullMode cull) override;
            platform::BlenderParamsInterface         *rdCreateBlenderParams(const platform::BlendMode blendMode) override; 
            platform::DepthParamsInterface           *rdCreateDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) override; 
            platform::SamplerInterface               *rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode) override; //!
            platform::ShaderConstantBufferInterface  *rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) override;
            platform::Texture2DInterface             *rdCreateTexture2D(unsigned char **imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::Texture2DInterface             *rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::RenderTargetInterface          *rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) override;
            platform::RenderTargetInterface          *rdGetDefaultRenderTarget() override;

            void  rdClearCurrentDepthBuffer(float depth = 1.0f) override;
            void  rdClearCurrentColorBuffer(const platform::color &c = platform::color(0.0f, 0.0f, 0.0f, 0.0f)) override;

            void  rdSetRenderTarget(const platform::RenderTargetInterface *rt) override;
            void  rdSetShader(const platform::ShaderInterface *shader) override;
            void  rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) override;
            void  rdSetBlenderParams(const platform::BlenderParamsInterface *params) override;
            void  rdSetDepthParams(const platform::DepthParamsInterface *params) override;
            void  rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) override;
            void  rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) override;
            void  rdSetTexture2D(platform::TextureSlot slot, const platform::Texture2DInterface *texture) override;
            void  rdSetScissorRect(math::p2d &topLeft, math::p2d &bottomRight) override;
            
            void  rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, platform::PrimitiveTopology topology, unsigned vertexCount) override;
            void  rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, platform::PrimitiveTopology topology, unsigned indexCount) override;
            void  rdPresent() override;

            bool  isInited() override;

        protected:
            const diag::LogInterface &_log;

            IXAudio2MasteringVoice   *_mastering;
            PhoneRenderTarget        *_curRenderTarget;
            PhoneSampler             *_defSampler;
            PhoneRenderTarget        _defRenderTarget;
            Agile                    <CoreWindow ^> _window;
            platform::Orientation    _orientation;
            
            float      _nativeWidth;
            float      _nativeHeight;
            float      _lastTextureWidth[platform::TEXTURE_UNITS_MAX];
            float      _lastTextureHeight[platform::TEXTURE_UNITS_MAX];

            unsigned   _syncInterval;
        };
    }
}

