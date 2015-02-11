
#include <vector>
#include <type_traits>
#pragma warning (disable : 4250) // for virtual inheritance in MSVS

namespace fg {
    namespace resources {
        enum class ResourceState {
            NOTLOADED = 0,
            LOADING = 1,
            LOADED = 2,
            CONSTRUCTING = 3,
            CONSTRUCTED = 4,
            INVALID = 5,
        };

        //---

        class ResourceInterface {
        public:
            virtual ~ResourceInterface() {}

            virtual bool valid() const = 0;
            virtual const fg::string &getFilePath() const = 0;
            virtual ResourceState getState() const = 0;
        };


        class ManagedResourceInterface : virtual public ResourceInterface {
        public:
            virtual unsigned getUnusedTimeMs() const = 0;            

            virtual bool unloadable() const = 0;
            virtual void freeBinary() = 0;
            virtual void setState(ResourceState state) = 0;
            virtual void setUnusedTimeMs(unsigned value) = 0;
            virtual void setBinary(void *binaryData, unsigned binarySize) = 0;
            
            // called on a separate thread when the file has loaded
            virtual void loaded(const diag::LogInterface &log) = 0;

            // resource is ready (formed), and need platform to create
            // return true if engine must keep binary in memory
            virtual bool constructed(const diag::LogInterface &log, platform::PlatformInterface &api) = 0;

            virtual void unloaded() = 0;
        };

        //---------------------------------------------------------------------

        class TextResourceInterface : virtual public ResourceInterface {
        public:
            virtual const char *getText() const = 0;
            virtual int   getSize() const = 0;
        };

        //---

        class ShaderResourceInterface : virtual public ResourceInterface {
        public:
            virtual const platform::ShaderInterface *getPlatformObject() const = 0;
        };

        //---

        class MeshInterface {
        public:
            mutable math::m4x4 tempMatrix;

            virtual ~MeshInterface() {}
            virtual const fg::string  &getName() const = 0;
            
            virtual const math::m4x4  &getLocalTransform() const = 0;
            virtual const math::quat  &getLocalRotation() const = 0;
            virtual const math::p3d   &getLocalPosition() const = 0;
            virtual const math::m4x4  &getStartTransform() const = 0;
            virtual const math::m4x4  &getInvStartTransform() const = 0;

            virtual unsigned  getPhysicVertexCount() const = 0;
            virtual unsigned  getPhysicIndexCount() const = 0;
            virtual unsigned  getPhysicTriangleCount() const = 0;
            virtual unsigned  getChildCount() const = 0;

            virtual const MeshInterface  *getChild(unsigned index) const = 0;
            virtual const math::p3d      *getPhysicVertexes() const = 0;
            virtual unsigned short       *getPhysicIndexes() const = 0;

            virtual void  getPhysicTriangle(unsigned index, math::p3d &oVertex1, math::p3d &oVertex2, math::p3d &oVertex3) const = 0;

            virtual bool  isPhysicPresent() const = 0;
            virtual bool  isVisible() const = 0;
            virtual bool  isSkinned() const = 0;
            virtual bool  isHelper() const = 0;

            virtual const MeshInterface  *getSkinnedBone(unsigned index) const = 0;
            virtual unsigned int         getSkinnedBoneCount() const = 0;

            virtual const math::p3d  &getMinBBoxPoint() const = 0;
            virtual const math::p3d  &getMaxBBoxPoint() const = 0;
            
            virtual const platform::IndexedVertexBufferInterface   *getMeshBuffer() const = 0;
            virtual const platform::ShaderConstantBufferInterface  *getSkinConstBuffer() const = 0;

            virtual unsigned getGeometryVertexCount() const = 0;
            virtual unsigned getGeometryIndexCount() const = 0;
        };

        class ModelResourceInterface : virtual public ResourceInterface {
        public:
            virtual const MeshInterface *getMesh(const fg::string &meshName) const = 0;
            virtual const MeshInterface *getRoot() const = 0;

            virtual const MeshInterface *getSkinMesh(const fg::string &meshName) const = 0;
            virtual const MeshInterface *getSkinMesh(unsigned index) const = 0;

            virtual unsigned getSkinMeshCount() const = 0;
            virtual unsigned getHelpersCount() const = 0;
            virtual unsigned getTotalMeshCount() const = 0;
            virtual unsigned getPhysicMeshCount() const = 0;

            virtual void  getHelpers(std::vector <const MeshInterface *> &out) const = 0;
            virtual void  getPhysicMeshes(std::vector <const MeshInterface *> &out) const = 0;
        };

        //---
        
        class Texture2DResourceInterface : virtual public ResourceInterface {
        public:
            virtual unsigned getWidth() const = 0;
            virtual unsigned getHeight() const = 0;

            virtual const platform::Texture2DInterface *getPlatformObject() const = 0;
        };

        //---

        struct FontCharInfo {
            platform::Texture2DInterface *texture;

            float   tu;
            float   tv;
            float   txWidth;    // width in texture (0..1)
            float   txHeight;   // height in texture (0..1)
            float   width;      // width, pixels
            float   height;     // height, pixels
            float   advance;    // 
            float   lsb;        // 
        };

        class FontResourceInterface : virtual public ResourceInterface {
        public:
            virtual void  cache(const char *mbcharsz, unsigned fontSize) = 0;
            virtual void  getChar(const char *mbChar, unsigned fontSize, FontCharInfo &out) const = 0;
            virtual void  getChar(wchar_t ch, unsigned fontSize, FontCharInfo &out) const = 0;
            virtual float getTextWidth(const char *text, unsigned fontSize) const = 0;
        };

        //---

        static const unsigned FG_MESH_MAX = 32;
        static const unsigned FG_MATERIAL_TEXTURE_MAX = 8;

        struct MaterialMeshParams {
            fg::string      shaderPath;
            fg::string      textureBinds[FG_MATERIAL_TEXTURE_MAX];
            unsigned        textureBindCount;
        };

        class MaterialResourceInterface : virtual public ResourceInterface {
        public:
            virtual const MaterialMeshParams *getMeshParams(const fg::string &meshName) const = 0;
        };

        //---

        struct ClipData {
            struct Frame {
                float   tu;
                float   tv;
            };

            float          width;
            float          height;
            float          centerX;
            float          centerY;
            Frame          *frames;
            math::p2d      *boundingCoords;
            unsigned int   frameCount;
            unsigned int   boundingCount;
            unsigned int   frameRate;
        };

        class ClipSetResourceInterface : virtual public ResourceInterface {
        public:
            virtual const ClipData  *getClip(const fg::string &name) const = 0;
        };

        //---

        class AnimationResourceInterface : virtual public ResourceInterface {
        public:
            virtual bool getMatrix(const fg::string &boneName, float animKoeff, math::m4x4 &oMatrix) const = 0;
            virtual bool getTransform(const fg::string &boneName, float animKoeff, math::p3d &oTranslation, math::quat &oRotation, math::p3d &oScaling) const = 0;
        };

        //---

        class SoundResourceInterface : virtual public ResourceInterface {
        public:
            virtual unsigned getSize() const = 0;
            virtual const unsigned char *getChunkPtr(unsigned offset) const = 0;
        };

        //---

        struct ResourcePtr {
            const ManagedResourceInterface *_resource;
            ResourcePtr(const ManagedResourceInterface *mres) : _resource(mres) {}

            template <typename DOWNTYPE> operator const DOWNTYPE *();
        };

        class ResourceManagerInterface {
        public:
            virtual ~ResourceManagerInterface() {}

            virtual void loadResourcesDir(const fg::string &dir, const callback <void ()> &completeCb, bool unloadable = true) = 0;
            virtual void loadResourcesList(const fg::string &resList, const callback <void ()> &completeCb, bool unloadable = true) = 0;
            virtual void unloadResourcesDir(const fg::string &dir) = 0;
            virtual void unloadResourcesList(const fg::string &resList) = 0;
            
            virtual ResourcePtr getResource(const fg::string &path) const = 0;
        };

        class EngineResourceManagerInterface : public ResourceManagerInterface {
        public:
            virtual ~EngineResourceManagerInterface() {}

            virtual void init() = 0;
            virtual void update(float frameTimeMs) = 0;
            virtual void destroy() = 0;
        };

    }
}