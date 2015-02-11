
namespace fg {
    namespace resources {
        class ModelResource : public ModelResourceInterface, public Resource {
        public:
            static const unsigned SKIN_MAX_BONES = 42;

            enum MESHTYPE {
                MESHTYPE_GEOMETRY = 0x1,
                MESHTYPE_HELPER = 0x2,
                MESHTYPE_SKIN = 0x4,
                MESHTYPE_VISIBLE = 0x8,
                MESHTYPE_USERDATA = 0x10,
                MESHTYPE_PHYSIC = 0x20,
                MESHTYPE_PHYSIC_OLD = 0x40,
            };

            class Mesh : public MeshInterface {
                friend class ModelResource;

            public:
                struct SkinContainer {
                    struct Bone {
                        float     weight;
                        char      boneName[32];
                    };
                    struct SkinVertex {
                        Bone      *bones;
                        unsigned  boneCount;
                    };

                    SkinVertex    *skinVertexes;
                    unsigned      skinVertexCount;

                    SkinContainer();
                    ~SkinContainer();

                    void load(ModelResource *mdl, Mesh *target, const byteform &data, unsigned vertexCount);
                };

                Mesh(ModelResource *container);
                ~Mesh() override;

                void load(const byteform &data, math::m4x4 *parentTransform, Mesh *root);
                void construct(platform::PlatformInterface &api);
                
                const fg::string  &getName() const override;

                const math::m4x4  &getLocalTransform() const override;
                const math::quat  &getLocalRotation() const override;
                const math::p3d   &getLocalPosition() const override;
                const math::m4x4  &getStartTransform() const override;
                const math::m4x4  &getInvStartTransform() const override;

                unsigned  getPhysicVertexCount() const override;
                unsigned  getPhysicIndexCount() const override;
                unsigned  getPhysicTriangleCount() const override;
                unsigned  getChildCount() const override;

                const MeshInterface  *getChild(unsigned index) const override;
                const math::p3d      *getPhysicVertexes() const override;
                unsigned short       *getPhysicIndexes() const override;

                void  getPhysicTriangle(unsigned index, math::p3d &oVertex1, math::p3d &oVertex2, math::p3d &oVertex3) const override;

                bool  isPhysicPresent() const override;
                bool  isVisible() const override;
                bool  isSkinned() const override;
                bool  isHelper() const override;

                const MeshInterface  *getSkinnedBone(unsigned index) const override;
                unsigned int         getSkinnedBoneCount() const override;

                const math::p3d  &getMinBBoxPoint() const override;
                const math::p3d  &getMaxBBoxPoint() const override;

                const platform::IndexedVertexBufferInterface *getMeshBuffer() const override;
                const platform::ShaderConstantBufferInterface  *getSkinConstBuffer() const override;
                
                unsigned getGeometryVertexCount() const override;
                unsigned getGeometryIndexCount() const override;
                                
            protected:
                ModelResource  *_container;

                unsigned int   _flags;
                fg::string     _name;
                math::quat     _localRotate;
                math::p3d      _localPosition;
                math::m4x4     _matCurrent;
                math::m4x4     _matInverse;
                math::m4x4     _matLocal;

                platform::IndexedVertexBufferInterface  *_meshBuffer;
                platform::ShaderConstantBufferInterface *_skinBuffer;
                VertexSkinnedNormal *_vertexArray;

                unsigned short   *_indexArray;
                unsigned int     _vertexArraySize;
                unsigned int     _indexArraySize;

                math::p3d        *_physicVertexes;
                unsigned short   *_physicIndexes;
                unsigned int     _physicVertexCount;
                unsigned int     _physicIndexCount;

                math::p3d        _minBB;
                math::p3d        _maxBB;

                const Mesh       *_skinBoneTable[SKIN_MAX_BONES];
                unsigned int     _skinBoneTableSize;
                
                std::vector      <Mesh *> _childs;

            private:
                Mesh(const Mesh &);
                Mesh &operator =(const Mesh &);
            };

            ModelResource(const char *path, bool unloadable) : Resource(path, unloadable), _root(nullptr), _helpersCount(0), _totalMeshCount(0), _physicMeshCount(0) {}
            ~ModelResource() override;

            void loaded(const diag::LogInterface &log) override;
            bool constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void unloaded() override;

            const MeshInterface *getMesh(const fg::string &meshName) const override;
            const MeshInterface *getRoot() const override;

            const MeshInterface *getSkinMesh(const fg::string &meshName) const override;
            const MeshInterface *getSkinMesh(unsigned index) const override;

            unsigned getSkinMeshCount() const override;
            unsigned getHelpersCount() const override;
            unsigned getTotalMeshCount() const override;
            unsigned getPhysicMeshCount() const override;

            void  getHelpers(std::vector <const MeshInterface *> &out) const override;
            void  getPhysicMeshes(std::vector <const MeshInterface *> &out) const override;

        protected:
            Mesh          *_root;
            std::vector   <Mesh *> _skinMeshes;
            unsigned      _helpersCount;
            unsigned      _totalMeshCount;
            unsigned      _physicMeshCount;
        };
    }
}

