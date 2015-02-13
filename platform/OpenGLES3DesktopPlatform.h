
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

        class ES3DesktopVertexBuffer : public PlatformObject, public platform::VertexBufferInterface {
        public:
            ES3DesktopVertexBuffer(platform::VertexType type, unsigned vcount, GLenum usage);
            ~ES3DesktopVertexBuffer() override;
            
            GLuint getVAO() const;
            
            void *lock() override;
            void unlock() override;

            void update(void *data) override;
            void release() override;

        public:
            GLenum    _usage;
            GLuint    _vao;
            GLuint    _vbo;
            unsigned  _length;
        };

        //---

        class ES3DesktopIndexedVertexBuffer : public PlatformObject, public platform::IndexedVertexBufferInterface {
        public:
            ES3DesktopIndexedVertexBuffer(platform::VertexType type, unsigned vcount, unsigned icount, GLenum usage);
            ~ES3DesktopIndexedVertexBuffer() override;

            GLuint getVAO() const;

            void *lockVertices() override;
            void *lockIndices() override;
            void unlockVertices() override;
            void unlockIndices() override;

            void updateVertices(void *data) override;
            void updateIndices(void *data) override;
            void release() override;

        protected:
            GLenum    _usage;
            GLuint    _vao;
            GLuint    _vbo;
            GLuint    _ibo;
            unsigned  _ilength;
            unsigned  _vlength;
        };

        //---

        class ES3DesktopRasterizerParams : public PlatformObject, public platform::RasterizerParamsInterface {
        public:
            ES3DesktopRasterizerParams(platform::CullMode cull);
            ~ES3DesktopRasterizerParams() override;

            void release() override;
            void set();

        protected:
            platform::CullMode  _cullMode;
        };

        //---

        class ES3DesktopBlenderParams : public PlatformObject, public platform::BlenderParamsInterface {
        public:
            ES3DesktopBlenderParams(const platform::BlendMode blendMode);
            ~ES3DesktopBlenderParams() override;

            void release() override;
            void set();

        protected:
            platform::BlendMode _blendMode;
        };

        //---

        class ES3DesktopDepthParams : public PlatformObject, public platform::DepthParamsInterface {
        public:
            ES3DesktopDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled);
            ~ES3DesktopDepthParams() override;

            void release() override;
            void set();

        protected:
            bool   _depthEnabled;
            bool   _depthWriteEnabled;            
            GLenum _cmpFunc;
        };

        //---

        class ES3DesktopSampler : public PlatformObject, public platform::SamplerInterface {
        public:
            ES3DesktopSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode);
            ~ES3DesktopSampler() override;

            void release() override;
            void set(platform::TextureSlot slot);

        protected:
            GLuint _self;
        };

        //---

        class ES3DesktopShader : public PlatformObject, public platform::ShaderInterface {
        public:
            ES3DesktopShader(const byteform &binary, const diag::LogInterface &log);
            ~ES3DesktopShader() override;

            void set();
            void release() override;
            
        public:
            GLuint _program;
            GLuint _vShader;
            GLuint _fShader;
            GLuint _textureLocations[platform::TEXTURE_UNITS_MAX];
        };

        //---

        class ES3DesktopShaderConstantBuffer : public PlatformObject, public platform::ShaderConstantBufferInterface {
        public:
            ES3DesktopShaderConstantBuffer(unsigned index, unsigned length);
            ~ES3DesktopShaderConstantBuffer() override;

            void set();
            void update(const void *data) const override;
            void release() override;

        public:
            GLuint    _ubo;
            unsigned  _length;
            unsigned  _index;
        };

        //---

        class ES3DesktopTexture2D : public PlatformObject, public platform::Texture2DInterface {
            friend class ES3DesktopRenderTarget;
            friend class ES3DesktopPlatform;

        public:
            ES3DesktopTexture2D();
            ES3DesktopTexture2D(platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            ~ES3DesktopTexture2D() override;

            unsigned getWidth() const override;
            unsigned getHeight() const override;
            unsigned getMipCount() const override;

            void update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) override;
            void *getNativeHandle() const override;

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

        class ES3DesktopRenderTarget : public PlatformObject, public platform::RenderTargetInterface {
            friend class ES3DesktopPlatform;

        public:
            ES3DesktopRenderTarget();
            ES3DesktopRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight);
            ~ES3DesktopRenderTarget() override;

            platform::Texture2DInterface *getDepthBuffer() override;
            platform::Texture2DInterface *getRenderBuffer(unsigned index) override;

            void  release() override;
            void  set();

        protected:
            unsigned _colorTargetCount;

            ES3DesktopTexture2D  _renderTexture[platform::RENDERTARGETS_MAX];
            ES3DesktopTexture2D  _depthTexture;
            GLuint _fbo;
        };

        //---

        struct DesktopInitParams : public platform::InitParams {
            float appWidth;
            float appHeight;
            HWND  hWindow;
        };

        class ES3DesktopPlatform : public platform::EnginePlatformInterface {
        public:
            ES3DesktopPlatform(const diag::LogInterface &log);

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
            
            const math::m3x3  &getInputTransform() const override;

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
            platform::SamplerInterface               *rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode) override; 
            platform::ShaderConstantBufferInterface  *rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) override;
            platform::Texture2DInterface             *rdCreateTexture2D(unsigned char **imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::Texture2DInterface             *rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) override;
            platform::RenderTargetInterface          *rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) override;
            platform::RenderTargetInterface          *rdGetDefaultRenderTarget() override;

            void  rdClearCurrentDepthBuffer(float depth = 1.0f) override;
            void  rdClearCurrentColorBuffer(const platform::color &c = platform::color(0.0f, 0.0f, 0.0f, 0.0f)) override;

            void  rdSetRenderTarget(const platform::RenderTargetInterface *rt) override;
            void  rdSetShader(const platform::ShaderInterface *vshader) override;
            void  rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) override;
            void  rdSetBlenderParams(const platform::BlenderParamsInterface *params) override;
            void  rdSetDepthParams(const platform::DepthParamsInterface *params) override;
            void  rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) override;
            void  rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) override;
            void  rdSetTexture2D(platform::TextureSlot, const platform::Texture2DInterface *texture) override;
            void  rdSetScissorRect(math::p2d &topLeft, math::p2d &bottomRight) override;

            void  rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, platform::PrimitiveTopology topology, unsigned vertexCount) override;
            void  rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, platform::PrimitiveTopology topology, unsigned indexCount) override;
            void  rdPresent() override;

            bool  isInited() override;

        protected:
            const diag::LogInterface  &_log;
            ES3DesktopRenderTarget    *_curRenderTarget;
            ES3DesktopRenderTarget    _defaultRenderTarget;
            
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
            ES3DesktopPlatform(const ES3DesktopPlatform &);
            ES3DesktopPlatform &operator =(const ES3DesktopPlatform &);
        };
    }
}