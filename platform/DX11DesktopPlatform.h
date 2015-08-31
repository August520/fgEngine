
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#include <xaudio2.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2.lib")

namespace fg {
    namespace dx11 {
        class DesktopPlatform;
        class PlatformObject {
        public:
            PlatformObject(DesktopPlatform *owner) : _owner(owner) {}
            virtual ~PlatformObject() {}
            virtual bool valid() const = 0;

        protected:
            DesktopPlatform *_owner;

        private:
            PlatformObject(const PlatformObject &);
            PlatformObject &operator =(const PlatformObject &);
        };

        //---

        class DesktopSoundEmitter : public PlatformObject, public platform::SoundEmitterInterface {
        public:
            DesktopSoundEmitter(DesktopPlatform *owner, unsigned sampleRate, unsigned channels);
            ~DesktopSoundEmitter() override;

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
                DesktopSoundEmitter *emitter;
                SoundCallback(DesktopSoundEmitter *iemitter) : emitter(iemitter) {}

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

        class DesktopVertexBuffer : public PlatformObject, public platform::VertexBufferInterface {
        public:
            DesktopVertexBuffer(DesktopPlatform *owner, platform::VertexType type, unsigned vcount, bool isDynamic, const void *data);
            ~DesktopVertexBuffer() override;

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

        class DesktopIndexedVertexBuffer : public PlatformObject, public platform::IndexedVertexBufferInterface {
        public:
            DesktopIndexedVertexBuffer(DesktopPlatform *owner, platform::VertexType type, unsigned vcount, unsigned icount, bool isDynamic, const void *vdata, const void *idata);
            ~DesktopIndexedVertexBuffer() override;

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

        class DesktopInstanceData : public PlatformObject, public platform::InstanceDataInterface {
        public:
            DesktopInstanceData(DesktopPlatform *owner, platform::InstanceDataType type, unsigned instanceCount);
            ~DesktopInstanceData() override;

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

        class DesktopRasterizerParams : public PlatformObject, public platform::RasterizerParamsInterface {
        public:
            DesktopRasterizerParams(DesktopPlatform *owner, platform::CullMode cull);
            ~DesktopRasterizerParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11RasterizerState *_self;
        };

        //--- 

        class DesktopBlenderParams : public PlatformObject, public platform::BlenderParamsInterface {
        public:
            DesktopBlenderParams(DesktopPlatform *owner, const platform::BlendMode blendMode);
            ~DesktopBlenderParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11BlendState *_self;
        };

        //--- 

        class DesktopDepthParams : public PlatformObject, public platform::DepthParamsInterface {
        public:
            DesktopDepthParams(DesktopPlatform *owner, bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled);
            ~DesktopDepthParams() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11DepthStencilState *_self;
        };

        //--- 

        class DesktopSampler : public PlatformObject, public platform::SamplerInterface {
        public:
            DesktopSampler(DesktopPlatform *owner, platform::TextureFilter filter, platform::TextureAddressMode addrMode);
            ~DesktopSampler() override;

            void release() override;
            void set(platform::TextureSlot slot);
            bool valid() const override;

        protected:
            ID3D11SamplerState *_self;
        };

        //--- 

        class DesktopShader : public PlatformObject, public platform::ShaderInterface {
        public:
            DesktopShader(DesktopPlatform *owner, const byteform &binary);
            ~DesktopShader() override;

            void release() override;
            void set();
            bool valid() const override;

        protected:
            ID3D11VertexShader  *_vsh;
            ID3D11PixelShader   *_psh;
            ID3D11InputLayout   *_layout;
        };
        
        //---

        class DesktopShaderConstantBuffer : public PlatformObject, public platform::ShaderConstantBufferInterface {
        public:
            DesktopShaderConstantBuffer(DesktopPlatform *owner, platform::ShaderConstBufferUsing appoint, unsigned byteWidth);
            ~DesktopShaderConstantBuffer() override;

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

        class DesktopTexture2D : public PlatformObject, public platform::Texture2DInterface {
            friend class DesktopRenderTarget;
            friend class DesktopPlatform;

        public:
            DesktopTexture2D();
            DesktopTexture2D(DesktopPlatform *owner, unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            DesktopTexture2D(DesktopPlatform *owner, platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            ~DesktopTexture2D() override;

            unsigned getWidth() const override;
            unsigned getHeight() const override;
            unsigned getMipCount() const override;

            void  update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) override;
            
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

        class DesktopRenderTarget : public PlatformObject, public platform::RenderTargetInterface {
            friend class DesktopPlatform;

        public:
            DesktopRenderTarget(DesktopPlatform *owner);
            DesktopRenderTarget(DesktopPlatform *owner, unsigned colorTargetCount, unsigned originWidth, unsigned originHeight);
            ~DesktopRenderTarget() override;

            platform::Texture2DInterface *getDepthBuffer() override;
            platform::Texture2DInterface *getRenderBuffer(unsigned index) override;
                        
            void  release() override;
            bool  valid() const override;
            void  set();

        protected:
            unsigned _colorTargetCount;

            DesktopTexture2D  _renderTexture[platform::RENDERTARGETS_MAX];
            DesktopTexture2D  _depthTexture;

            ID3D11RenderTargetView  *_rtView[platform::RENDERTARGETS_MAX];
            ID3D11DepthStencilView  *_depthView;
        };

        //---

        struct DesktopInitParams : public platform::InitParams {
            float appWidth;
            float appHeight;
            HWND  hWindow;
        };

        class DesktopPlatform : public platform::EnginePlatformInterface {
        public:
            ID3D11Device         *_device;
            ID3D11DeviceContext  *_context;
            IDXGISwapChain       *_swapChain;
            IXAudio2             *_audio;

            DesktopPlatform(const diag::LogInterface &log);

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
            void  resize(float width, float height) override {}
            
            const math::m3x3  &getInputTransform() const override;

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

            IXAudio2MasteringVoice  *_mastering;
            DesktopRenderTarget     *_curRenderTarget;
            DesktopSampler          *_defSampler;
            DesktopRenderTarget     _defRenderTarget;

            float      _nativeWidth;
            float      _nativeHeight;
            float      _lastTextureWidth[platform::TEXTURE_UNITS_MAX];
            float      _lastTextureHeight[platform::TEXTURE_UNITS_MAX];

            unsigned   _syncInterval;
        };
    }
}

