
#include "../../ogles/egl.h"
#include "../../ogles/gl3.h"

#pragma comment(lib, "ogles/libEGL.lib")
#pragma comment(lib, "ogles/libGLESv2.lib")

namespace fg {
    namespace opengl {
        class PlatformObject {
        public:
            PlatformObject() {}
            virtual ~PlatformObject() {}

        private:
            PlatformObject(const PlatformObject &);
            PlatformObject &operator =(const PlatformObject &);
        };

        //---

        class ES3DesktopWin32VertexBuffer : public PlatformObject, public platform::VertexBufferInterface {
        public:
            ES3DesktopWin32VertexBuffer(platform::VertexType type, unsigned vcount, GLenum usage);
            ~ES3DesktopWin32VertexBuffer() override;
            
            GLuint getVAO() const;
            
            void *lock() override;
            void unlock() override;
            void release() override;

            unsigned getLength() const;

        protected:
            GLenum    _usage;
            GLuint    _vao;
            GLuint    _vbo;
            unsigned  _length;
            unsigned  _attribsCount;
        };

        //---

        class ES3DesktopWin32IndexedVertexBuffer : public PlatformObject, public platform::IndexedVertexBufferInterface {
        public:
            ES3DesktopWin32IndexedVertexBuffer(platform::VertexType type, unsigned vcount, unsigned icount, GLenum usage);
            ~ES3DesktopWin32IndexedVertexBuffer() override;

            GLuint getVAO() const;

            void *lockVertices() override;
            void *lockIndices() override;
            void unlockVertices() override;
            void unlockIndices() override;
            void release() override;

            unsigned getVertexDataLength() const;
            unsigned getIndexDataLength() const;

        protected:
            GLenum    _usage;
            GLuint    _vao;
            GLuint    _vbo;
            GLuint    _ibo;
            unsigned  _ilength;
            unsigned  _vlength;
            unsigned  _attribsCount;
        };

        //---

        class ES3DesktopWin32InstanceData : public PlatformObject, public platform::InstanceDataInterface {
        public:
            ES3DesktopWin32InstanceData(platform::InstanceDataType type, unsigned instanceCount);
            ~ES3DesktopWin32InstanceData() override;

            GLuint getVBO() const;

            void *lock() override;
            void unlock() override;
            
            void update(const void *data, unsigned instanceCount) override;
            void release() override;

            platform::InstanceDataType getType() const;

        protected:
            GLuint    _vbo;
            unsigned  _length;
            platform::InstanceDataType _type;
        };

        //---

        class ES3DesktopWin32RasterizerParams : public PlatformObject, public platform::RasterizerParamsInterface {
        public:
            ES3DesktopWin32RasterizerParams(platform::CullMode cull);
            ~ES3DesktopWin32RasterizerParams() override;

            void release() override;
            void set();

        protected:
            platform::CullMode  _cullMode;
        };

        //---

        class ES3DesktopWin32BlenderParams : public PlatformObject, public platform::BlenderParamsInterface {
        public:
            ES3DesktopWin32BlenderParams(const platform::BlendMode blendMode);
            ~ES3DesktopWin32BlenderParams() override;

            void release() override;
            void set();

        protected:
            platform::BlendMode _blendMode;
        };

        //---

        class ES3DesktopWin32DepthParams : public PlatformObject, public platform::DepthParamsInterface {
        public:
            ES3DesktopWin32DepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled);
            ~ES3DesktopWin32DepthParams() override;

            void release() override;
            void set();

        protected:
            bool   _depthEnabled;
            bool   _depthWriteEnabled;            
            GLenum _cmpFunc;
        };

        //---

        class ES3DesktopWin32Sampler : public PlatformObject, public platform::SamplerInterface {
        public:
            ES3DesktopWin32Sampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode, float minLod, float bias);
            ~ES3DesktopWin32Sampler() override;

            void release() override;
            void set(platform::TextureSlot slot);

        protected:
            GLuint _self;
        };

        //---

        class ES3DesktopWin32Shader : public PlatformObject, public platform::ShaderInterface {
        public:
            ES3DesktopWin32Shader(const byteinput &binary, const diag::LogInterface &log);
            ~ES3DesktopWin32Shader() override;

            void set();
            void release() override;
            
        public:
            GLuint _program;
            GLuint _vShader;
            GLuint _fShader;
            GLuint _textureLocations[platform::TEXTURE_UNITS_MAX];
        };

        //---

        class ES3DesktopWin32ShaderConstantBuffer : public PlatformObject, public platform::ShaderConstantBufferInterface {
        public:
            ES3DesktopWin32ShaderConstantBuffer(unsigned index, unsigned length);
            ~ES3DesktopWin32ShaderConstantBuffer() override;

            void set();
            void update(const void *data, unsigned byteWidth) override;
            void release() override;

        protected:
            GLuint    _ubo;
            unsigned  _length;
            unsigned  _index;
        };

        //---

        class ES3DesktopWin32Texture2D : public PlatformObject, public platform::Texture2DInterface {
            friend class ES3DesktopWin32RenderTarget;
            friend class ES3DesktopWin32Platform;

        public:
            ES3DesktopWin32Texture2D();
            ES3DesktopWin32Texture2D(platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            ES3DesktopWin32Texture2D(unsigned char *const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount, platform::TextureFormat fmt);
            ~ES3DesktopWin32Texture2D() override;

            unsigned getWidth() const override;
            unsigned getHeight() const override;
            unsigned getMipCount() const override;

            void update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) override;

            void set(platform::TextureSlot slot);
            void release() override;

        protected:
            platform::TextureFormat _format;
            GLuint    _texture;
            unsigned  _width;
            unsigned  _height;
            unsigned  _mipCount;
        };

        //---

        class ES3DesktopWin32TextureCube : public PlatformObject, public platform::TextureCubeInterface {
            friend class DesktopRenderTarget;
            friend class ES3DesktopWin32Platform;

        public:
            ES3DesktopWin32TextureCube(ES3DesktopWin32Platform *owner, unsigned char **imgMipsBinaryData[6], unsigned originSize, unsigned mipCount, platform::TextureFormat format);
            ~ES3DesktopWin32TextureCube() override;

            void  release() override;
            void  set(platform::TextureSlot slot);

        protected:
            platform::TextureFormat   _format;
            GLuint    _texture;
        };

        //---

        class ES3DesktopWin32RenderTarget : public PlatformObject, public platform::RenderTargetInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ES3DesktopWin32RenderTarget();
            ES3DesktopWin32RenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight);
            ~ES3DesktopWin32RenderTarget() override;

            platform::Texture2DInterface *getDepthBuffer() override;
            platform::Texture2DInterface *getRenderBuffer(unsigned index) override;
            unsigned getRenderBufferCount() const override;

            void  release() override;
            void  set();

        protected:
            unsigned _colorTargetCount;

            ES3DesktopWin32Texture2D  _renderTexture[platform::RENDERTARGETS_MAX];
            ES3DesktopWin32Texture2D  _depthTexture;
            GLuint _fbo;
        };

        //---

        struct DesktopWin32InitParams : public platform::InitParams {
            float appWidth;
            float appHeight;
            HWND  hWindow;
        };

        class ES3DesktopWin32Platform : public platform::EnginePlatformInterface {
        public:
            ES3DesktopWin32Platform(const diag::LogInterface &log);

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
            void  resize(float width, float height) override;
            
            const math::m3x3  &getInputTransform() const override;

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
            platform::Texture2DInterface             *rdCreateTexture2D(unsigned char *const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount, platform::TextureFormat fmt) override;
            platform::Texture2DInterface             *rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::TextureCubeInterface           *rdCreateTextureCube(unsigned char **imgMipsBinaryData[6], unsigned originSize, unsigned mipCount, platform::TextureFormat fmt) override;
            platform::RenderTargetInterface          *rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) override;
            platform::RenderTargetInterface          *rdGetDefaultRenderTarget() override;

            void  rdClearCurrentDepthBuffer(float depth = 1.0f) override;
            void  rdClearCurrentColorBuffer(const fg::color &c) override;

            void  rdSetRenderTarget(const platform::RenderTargetInterface *rt) override;
            void  rdSetShader(const platform::ShaderInterface *vshader) override;
            void  rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) override;
            void  rdSetBlenderParams(const platform::BlenderParamsInterface *params) override;
            void  rdSetDepthParams(const platform::DepthParamsInterface *params) override;
            void  rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) override;
            void  rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) override;
            void  rdSetTexture2D(platform::TextureSlot, const platform::Texture2DInterface *texture) override;
            void  rdSetTextureCube(platform::TextureSlot slot, const platform::TextureCubeInterface *texture) override;
            void  rdSetScissorRect(const math::p2d &topLeft, const math::p2d &bottomRight) override;

            void  rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned vertexCount, unsigned instanceCount) override;
            void  rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned indexCount, unsigned instanceCount) override;
            void  rdPresent() override;

            bool  isInited() const override;

        protected:
            const diag::LogInterface     &_log;
            ES3DesktopWin32RenderTarget  *_curRenderTarget;
            ES3DesktopWin32RenderTarget  _defaultRenderTarget;
            
            HWND        _hwnd;
            HDC         _hdc;
            EGLDisplay  _eglDisplay;
            EGLConfig	_eglConfig;
            EGLSurface	_eglSurface;
            EGLContext	_eglContext;

            float       _nativeWidth;
            float       _nativeHeight;

            float      _lastTextureWidth[platform::TEXTURE_UNITS_MAX];
            float      _lastTextureHeight[platform::TEXTURE_UNITS_MAX];

        private:
            ES3DesktopWin32Platform(const ES3DesktopWin32Platform &);
            ES3DesktopWin32Platform &operator =(const ES3DesktopWin32Platform &);
        };
    }
}