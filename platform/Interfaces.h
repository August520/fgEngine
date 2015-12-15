
#include <string>

namespace fg {
    namespace platform {
        const unsigned TEXTURE_UNITS_MAX = 8;
        const unsigned RENDERTARGETS_MAX = 4;

        enum class Orientation {
            PORTRAIT = 0,
            ALBUM    = 1,
        };

        enum class PrimitiveTopology {
            LINE_LIST      = 0,
            LINE_STRIP     = 1,
            TRIANGLE_LIST  = 2,
            TRIANGLE_STRIP = 3,
        };

        enum class VertexType {
            SIMPLE            = 0,   // p3d pos
            TEXTURED          = 1,   // p3d pos, p2d uv
            NORMAL            = 2,   // p3d pos, p2d uv, p3d nrm, p3d binormal, p3d tangent
            SKIN_NORMAL       = 3,   // p3d pos, p2d uv, p3d nrm, p3d binormal, p3d tangent, p4d boneIndexes, p4d boneWeights
        };

        enum class InstanceDataType {
            DEFAULT         = 0,     // m4x4 transform, p4d color
            DISPLAY_OBJECT  = 1,     // p3d pos, float isGrey, p4d color
        };

        enum class TextureFormat {
            RGBA8   = 0,
            RED8    = 1,
            UNKNOWN = -1,
        };

        enum class TextureFilter {
            POINT       = 0,
            LINEAR      = 1,
            ANISOTROPIC = 2,
            SHADOW      = 3,
        };

        enum class TextureAddressMode {
            WRAP  = 0,
            CLAMP = 1,
        };

        enum class TextureSlot {
            TEXTURE0 = 0,
            TEXTURE1 = 1,
            TEXTURE2 = 2,
            TEXTURE3 = 3,
            TEXTURE4 = 4,
            TEXTURE5 = 5,
            TEXTURE6 = 6,
            TEXTURE7 = 7,
        };

        enum class CullMode {
            NONE  = 0,
            BACK  = 1,
            FRONT = 2,
        };

        enum class BlendMode {
            NOBLEND    = 0,
            ALPHA_LERP = 1,
            ALPHA_ADD  = 2,
        };

        enum class DepthFunc {
            NEVER         = 0,
            LESS          = 1,
            EQUAL         = 2,
            LESS_EQUAL    = 3,
            GREATER       = 4,
            NOT_EQUAL     = 5,
            GREATER_EQUAL = 6,
            ALWAYS        = 7,
        };

        enum class ShaderConstBufferUsing {
            FRAME_DATA            = 0,
            SKIN_DATA             = 1,
            MATERIAL_DATA         = 2,   
            ADDITIONAL_DATA       = 3,
        };

        class InitParams {
        public:
            float         dpi;
            float         scrWidth;
            float         scrHeight;
            unsigned      syncInterval;

        protected:
            InitParams() {}
        };

        class SoundEmitterInterface {
        public:
            virtual ~SoundEmitterInterface() {}
            virtual void pushBuffer(const char *data, unsigned samples) = 0;
            virtual void setBufferEndCallback(void (*cb)(void *), void *userPtr = nullptr) = 0;
            virtual void setVolume(float volume) = 0;
            virtual void setWorldTransform(const math::m4x4 &matrix) = 0;
            virtual void play() = 0;
            virtual void stop() = 0;
            virtual void release() = 0;
        };

        class VertexBufferInterface {
        public:
            virtual ~VertexBufferInterface() {}

            virtual void *lock() = 0;
            virtual void unlock() = 0;

            virtual void release() = 0;
        };

        class IndexedVertexBufferInterface {
        public:
            virtual ~IndexedVertexBufferInterface() {}

            virtual void *lockVertices() = 0; 
            virtual void *lockIndices() = 0;
            virtual void unlockVertices() = 0;
            virtual void unlockIndices() = 0;

            virtual void release() = 0;
        };

        class InstanceDataInterface {
        public:
            virtual ~InstanceDataInterface() {}

            virtual void *lock() = 0;
            virtual void unlock() = 0;
            
            virtual void update(const void *data, unsigned instanceCount) = 0;
            virtual void release() = 0;
        };

        class RasterizerParamsInterface {
        public:
            virtual ~RasterizerParamsInterface() {}
            virtual void release() = 0;
        };

        class BlenderParamsInterface {
        public:
            virtual ~BlenderParamsInterface() {}
            virtual void release() = 0;
        };

        class DepthParamsInterface {
        public:
            virtual ~DepthParamsInterface() {}
            virtual void release() = 0;
        };

        class SamplerInterface {
        public:
            virtual ~SamplerInterface() {}
            virtual void release() = 0;
        };

        class ShaderInterface {
        public:
            virtual ~ShaderInterface() {}
            virtual void release() = 0;
        };

        class ShaderConstantBufferInterface {
        public:
            virtual ~ShaderConstantBufferInterface() {}
            virtual void update(const void *data, unsigned byteWidth = 0) = 0;
            virtual void release() = 0;
        };

        class Texture2DInterface {
        public:
            virtual ~Texture2DInterface() {}
            virtual unsigned getWidth() const = 0;
            virtual unsigned getHeight() const = 0;
            virtual unsigned getMipCount() const = 0;

            virtual void update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) = 0;
            virtual void release() = 0;
        };

        class RenderTargetInterface {
        public:
            virtual ~RenderTargetInterface() {}
            virtual Texture2DInterface *getDepthBuffer() = 0;
            virtual Texture2DInterface *getRenderBuffer(unsigned index) = 0;
            virtual unsigned getRenderBufferCount() const = 0;
            
            virtual void  release() = 0;
        };

        //---------------------------------------------------------------------

        class PlatformInterface {
        public:
            virtual ~PlatformInterface() {}
            
            virtual float getScreenWidth() const = 0;
            virtual float getScreenHeight() const = 0;
            virtual float getCurrentRTWidth() const = 0;
            virtual float getCurrentRTHeight() const = 0;

            virtual unsigned  getMemoryUsing() const = 0;
            virtual unsigned  getMemoryLimit() const = 0;
            virtual unsigned  long long getTimeMs() const = 0;

            virtual void  sndSetGlobalVolume(float volume) = 0;

            virtual SoundEmitterInterface         *sndCreateEmitter(unsigned sampleRate, unsigned channels) = 0;

            virtual VertexBufferInterface         *rdCreateVertexBuffer(VertexType vtype, unsigned vcount, bool isDynamic, const void *data = nullptr) = 0;
            virtual IndexedVertexBufferInterface  *rdCreateIndexedVertexBuffer(VertexType vtype, unsigned vcount, unsigned ushortIndexCount, bool isDynamic, const void *vdata = nullptr, const void *idata = nullptr) = 0;
            virtual InstanceDataInterface         *rdCreateInstanceData(InstanceDataType type, unsigned instanceCount) = 0;
            virtual ShaderInterface               *rdCreateShader(const byteinput &binary) = 0;
            virtual RasterizerParamsInterface     *rdCreateRasterizerParams(CullMode cull) = 0;
            virtual BlenderParamsInterface        *rdCreateBlenderParams(const BlendMode blendMode) = 0;
            virtual DepthParamsInterface          *rdCreateDepthParams(bool depthEnabled, DepthFunc compareFunc, bool depthWriteEnabled) = 0;
            virtual SamplerInterface              *rdCreateSampler(TextureFilter filter, TextureAddressMode addrMode) = 0;
            virtual ShaderConstantBufferInterface *rdCreateShaderConstantBuffer(ShaderConstBufferUsing appoint, unsigned byteWidth) = 0;
            virtual Texture2DInterface            *rdCreateTexture2D(unsigned char * const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) = 0;
            virtual Texture2DInterface            *rdCreateTexture2D(TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) = 0;
            virtual RenderTargetInterface         *rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) = 0;
            virtual RenderTargetInterface         *rdGetDefaultRenderTarget() = 0;

            virtual void  rdClearCurrentDepthBuffer(float depth = 1.0f) = 0;
            virtual void  rdClearCurrentColorBuffer(const color &c = color(0.0f, 0.0f, 0.0f, 0.0f)) = 0;

            virtual void  rdSetRenderTarget(const RenderTargetInterface *rt) = 0;
            virtual void  rdSetShader(const ShaderInterface *vshader) = 0;
            virtual void  rdSetRasterizerParams(const RasterizerParamsInterface *params) = 0;
            virtual void  rdSetBlenderParams(const BlenderParamsInterface *params) = 0;
            virtual void  rdSetDepthParams(const DepthParamsInterface *params) = 0;
            virtual void  rdSetSampler(TextureSlot slot, const SamplerInterface *sampler) = 0;
            virtual void  rdSetShaderConstBuffer(const ShaderConstantBufferInterface *cbuffer) = 0;
            virtual void  rdSetTexture2D(TextureSlot slot, const Texture2DInterface *texture = nullptr) = 0;
            virtual void  rdSetScissorRect(math::p2d &topLeft, math::p2d &bottomRight) = 0;

            virtual void  rdDrawGeometry(const VertexBufferInterface *vbuffer, const InstanceDataInterface *instanceData, PrimitiveTopology topology, unsigned vertexCount, unsigned instanceCount = 1) = 0;
            virtual void  rdDrawIndexedGeometry(const IndexedVertexBufferInterface *ivbuffer, const InstanceDataInterface *instanceData, PrimitiveTopology topology, unsigned indexCount, unsigned instanceCount = 1) = 0;
        };

        class EnginePlatformInterface : public PlatformInterface {
        public:
            virtual bool  init(const InitParams &initParams) = 0;
            virtual void  destroy() = 0;
            virtual void  updateOrientation() = 0;
            virtual void  resize(float width, float height) = 0;

            virtual float getTextureWidth(TextureSlot slot) const = 0;
            virtual float getTextureHeight(TextureSlot slot) const = 0;

            virtual const math::m3x3  &getInputTransform() const = 0;
            
            virtual void  fsFormFilesList(const char *path, std::string &out) = 0;
            virtual bool  fsLoadFile(const char *path, void **oBinaryDataPtr, unsigned *oSize) = 0;
            virtual bool  fsSaveFile(const char *path, void *iBinaryDataPtr, unsigned iSize) = 0;

            virtual void  rdPresent() = 0;
            virtual bool  isInited() const = 0;

        };
    }
}




