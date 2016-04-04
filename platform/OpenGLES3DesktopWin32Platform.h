
#include "../../ogles/egl.h"
#include "../../ogles/gl3.h"

#pragma comment(lib, "ogles/libEGL.lib")
#pragma comment(lib, "ogles/libGLESv2.lib")

namespace fg {
    namespace opengl {
        class PlatformObject : public uncopyable{
        public:
            PlatformObject() {}
            virtual ~PlatformObject() {}
            virtual bool valid() const {
                return true;
            }
        };

        //---

        class ES3DesktopWin32VertexBuffer : public PlatformObject, public platform::VertexBufferInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32VertexBuffer() override;
            
            GLuint getVAO() const;
            
            void *lock() override;
            void unlock() override;
            void release() override;
            bool valid() const override;

            unsigned getLength() const;

        protected:
            ES3DesktopWin32VertexBuffer(platform::VertexType type, unsigned vcount, GLenum usage);
            
            GLenum    _usage;
            GLuint    _vao = 0;
            GLuint    _vbo = 0;
            unsigned  _length = 0;
            unsigned  _attribsCount = 0;
        };

        //---

        class ES3DesktopWin32IndexedVertexBuffer : public PlatformObject, public platform::IndexedVertexBufferInterface {
            friend class ES3DesktopWin32Platform;
        
        public:
            ~ES3DesktopWin32IndexedVertexBuffer() override;

            GLuint getVAO() const;

            void *lockVertices() override;
            void *lockIndices() override;
            void unlockVertices() override;
            void unlockIndices() override;
            void release() override;
            bool valid() const override;

            unsigned getVertexDataLength() const;
            unsigned getIndexDataLength() const;

        protected:
            ES3DesktopWin32IndexedVertexBuffer(platform::VertexType type, unsigned vcount, unsigned icount, GLenum usage);
            
            GLenum    _usage;
            GLuint    _vao = 0;
            GLuint    _vbo = 0;
            GLuint    _ibo = 0;
            unsigned  _ilength = 0;
            unsigned  _vlength = 0;
            unsigned  _attribsCount = 0;
        };

        //---

        class ES3DesktopWin32InstanceData : public PlatformObject, public platform::InstanceDataInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32InstanceData() override;

            GLuint getVBO() const;

            void *lock() override;
            void unlock() override;
            
            void update(const void *data, unsigned instanceCount) override;
            void release() override;
            bool valid() const override;

            platform::InstanceDataType getType() const;

        protected:
            ES3DesktopWin32InstanceData(platform::InstanceDataType type, unsigned instanceCount);
            
            GLuint    _vbo = 0;
            unsigned  _length = 0;
            platform::InstanceDataType _type;
        };

        //---

        class ES3DesktopWin32RasterizerParams : public PlatformObject, public platform::RasterizerParamsInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32RasterizerParams() override;

            void release() override;
            void set();

        protected:
            ES3DesktopWin32RasterizerParams(platform::CullMode cull);
            platform::CullMode  _cullMode;
        };

        //---

        class ES3DesktopWin32BlenderParams : public PlatformObject, public platform::BlenderParamsInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32BlenderParams() override;

            void release() override;
            void set();

        protected:
            ES3DesktopWin32BlenderParams(const platform::BlendMode blendMode);
            platform::BlendMode _blendMode;
        };

        //---

        class ES3DesktopWin32DepthParams : public PlatformObject, public platform::DepthParamsInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32DepthParams() override;

            void release() override;
            void set();

        protected:
            ES3DesktopWin32DepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled);
            
            bool   _depthEnabled;
            bool   _depthWriteEnabled;            
            GLenum _cmpFunc;
        };

        //---

        class ES3DesktopWin32Sampler : public PlatformObject, public platform::SamplerInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32Sampler() override;

            void release() override;
            void set(platform::TextureSlot slot);
            bool valid() const override;

        protected:
            ES3DesktopWin32Sampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode, float minLod, float bias);
            GLuint _self = 0;
        };

        //---

        class ES3DesktopWin32Shader : public PlatformObject, public platform::ShaderInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32Shader() override;

            void set();
            void release() override;
            bool valid() const override;
            
        public:
            ES3DesktopWin32Shader(const byteinput &binary, const diag::LogInterface &log);
            
            GLuint _program = 0;
            GLuint _vShader = 0;
            GLuint _fShader = 0;
            GLuint _textureLocations[FG_TEXTURE_UNITS_MAX] = {0};
        };

        //---

        class ES3DesktopWin32ShaderConstantBuffer : public PlatformObject, public platform::ShaderConstantBufferInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32ShaderConstantBuffer() override;

            void set();
            void update(const void *data, unsigned byteWidth) override;
            void release() override;
            bool valid() const override;

        protected:
            ES3DesktopWin32ShaderConstantBuffer(unsigned index, unsigned length);
            
            GLuint    _ubo = 0;
            unsigned  _length = 0;
            unsigned  _index;
        };

        //---

        class ES3DesktopWin32Texture2D : public PlatformObject, public platform::Texture2DInterface {
            friend class ES3DesktopWin32RenderTarget;
            friend class ES3DesktopWin32CubeRenderTarget;
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32Texture2D() override;

            unsigned getWidth() const override;
            unsigned getHeight() const override;
            unsigned getMipCount() const override;

            void update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) override;

            void set(platform::TextureSlot slot);
            void release() override;
            bool valid() const override;

        protected:
            ES3DesktopWin32Texture2D();
            ES3DesktopWin32Texture2D(platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount);
            ES3DesktopWin32Texture2D(unsigned char *const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount, platform::TextureFormat fmt);
            
            platform::TextureFormat _format;
            GLuint    _texture = 0;
            unsigned  _width = 0;
            unsigned  _height = 0;
            unsigned  _mipCount = 0;
        };

        //---

        class ES3DesktopWin32TextureCube : public PlatformObject, public platform::TextureCubeInterface {
            friend class ES3DesktopWin32CubeRenderTarget;
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32TextureCube() override;

            void release() override;
            void set(platform::TextureSlot slot);
            bool valid() const override;

        protected:
            ES3DesktopWin32TextureCube();
            ES3DesktopWin32TextureCube(unsigned char **imgMipsBinaryData[6], unsigned originSize, unsigned mipCount, platform::TextureFormat format);
            
            platform::TextureFormat _format;
            GLuint _texture = 0;
        };

        //---

        class ES3DesktopWin32RenderTarget : public PlatformObject, public platform::RenderTargetInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32RenderTarget() override;

            const platform::Texture2DInterface *getDepthBuffer() const override;
            const platform::Texture2DInterface *getRenderBuffer(unsigned index) const override;

            unsigned getRenderBufferCount() const override;
            unsigned getWidth() const override;
            unsigned getHeight() const override;

            void  release() override;
            void  set();
            bool  valid() const override;

        protected:
            ES3DesktopWin32RenderTarget();
            ES3DesktopWin32RenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight, platform::RenderTargetType type);

            platform::RenderTargetType _type = platform::RenderTargetType::Normal;
            unsigned _colorTargetCount = 0;
            unsigned _width = 0;
            unsigned _height = 0;

            ES3DesktopWin32Texture2D   _renderTextures[FG_RENDERTARGETS_MAX];
            ES3DesktopWin32Texture2D   _depthTexture;
            GLuint _fbo = 0;
        };

        //---

        class ES3DesktopWin32CubeRenderTarget : public PlatformObject, public platform::CubeRenderTargetInterface {
            friend class ES3DesktopWin32Platform;

        public:
            ~ES3DesktopWin32CubeRenderTarget() override;

            const platform::Texture2DInterface *getDepthBuffer() const override;
            const platform::TextureCubeInterface *getRenderBuffer() const override;
            
            unsigned getSize() const override;

            void  set(unsigned faceIndex);
            void  release() override;
            bool  valid() const override;

        protected:
            ES3DesktopWin32CubeRenderTarget();
            ES3DesktopWin32CubeRenderTarget(unsigned originSize, platform::RenderTargetType type);

            platform::RenderTargetType _type = platform::RenderTargetType::Normal;
            unsigned _size = 0;

            ES3DesktopWin32TextureCube _renderCube;
            ES3DesktopWin32Texture2D   _depthTexture;
            GLuint _fbo = 0;
        };

        //---

        struct DesktopWin32InitParams : public platform::InitParams {
            float appWidth;
            float appHeight;
            HWND  hWindow;
        };

        class ES3DesktopWin32Platform : public platform::EnginePlatformInterface, public uncopyable {
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
            platform::RenderTargetInterface          *rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight, platform::RenderTargetType type) override;
            platform::CubeRenderTargetInterface      *rdCreateCubeRenderTarget(unsigned originSize, platform::RenderTargetType type) override;
            platform::RenderTargetInterface          *rdGetDefaultRenderTarget() override;

            void  rdClearCurrentDepthBuffer(float depth = 1.0f) override;
            void  rdClearCurrentColorBuffer(const fg::color &c) override;

            void  rdSetRenderTarget(const platform::RenderTargetInterface *rt) override;
            void  rdSetCubeRenderTarget(const platform::CubeRenderTargetInterface *rt, unsigned faceIndex) override;
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
            ES3DesktopWin32RenderTarget  _defaultRenderTarget;

            unsigned    _curRTWidth = 1;
            unsigned    _curRTHeight = 1;
            unsigned    _curRTColorTargetCount = 1;
            
            HWND        _hwnd = 0;
            HDC         _hdc = 0;
            EGLDisplay  _eglDisplay = nullptr;
            EGLConfig	_eglConfig = nullptr;
            EGLSurface	_eglSurface = nullptr;
            EGLContext	_eglContext = nullptr;

            float       _nativeWidth = 0.0f;
            float       _nativeHeight = 0.0f;

            float       _lastTextureWidth[FG_TEXTURE_UNITS_MAX];
            float       _lastTextureHeight[FG_TEXTURE_UNITS_MAX];
        };
    }
}

