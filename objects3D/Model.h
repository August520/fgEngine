
namespace fg {
    const unsigned FG_ANIM_LAYERS_MAX = 4;

    namespace object3d {
        class Animator final {
        public:
            Animator();
            ~Animator();

            void getMatrix(const fg::string &boneName, math::m4x4 &oMatrix);
            void updateAnimation(float frameTimeMs);
            void updateResources(resources::ResourceManagerInterface &resMan);
            void playAnim(const fg::string &animResourcePath, float animLenMs, float animOffsetMs, float smoothTimeMs, AnimationLayer ilayer);

            unsigned getActiveLayersCount() const;

        private:
            struct Layer {
                const resources::AnimationResourceInterface *curAnimation;
                const resources::AnimationResourceInterface *nextAnimation;
            
                fg::string  curAnimResourcePath;
                fg::string  nextAnimResourcePath;
                
                float curAnimTimeLen;
                float curAnimTimePass;
                float nextAnimTimeLen;
                float nextAnimTimePass;
                float smoothTime;

                Layer() : curAnimation(nullptr), nextAnimation(nullptr), curAnimTimeLen(0), curAnimTimePass(0), nextAnimTimeLen(0), nextAnimTimePass(0), smoothTime(0) {}
            };

            Layer     _layers[FG_ANIM_LAYERS_MAX];
            unsigned  _activeLayers;
            
            Animator(const Animator &);
            Animator &operator =(const Animator &);
        };

        //---

        class Model3D : public Model3DInterface, public RenderObject {
        public:
            class MeshData : public Model3DInterface::MeshComponentInterface {
                friend class Model3D;

            public:
                MeshData();
                ~MeshData() override;

                const math::m4x4 &getFullTransform() const override;
                const math::m4x4 *getSkinMatrixArray() const override;

                unsigned getSkinMatrixCount() const override;
                unsigned getTextureBindCount() const override;

                const resources::Texture2DResourceInterface *getTextureBind(unsigned bindIndex) const override;
                const resources::ShaderResourceInterface *getShader() const override;
                const resources::MeshInterface *getMesh() const override;

                bool isSkinned() const override;
                bool isVisible() const override;

                bool isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

            protected:
                bool      _skinned;
                bool      _visible;
                bool      _addtrfm;
                MeshData  *_childs;
                
                unsigned  _childCount;
                unsigned  _skinMatrixCount;

                math::m4x4  _fullTransform;
                math::m4x4  _additionalTransform;                
                math::m4x4  *_skinMatrixes;

                const resources::MeshInterface               *_mesh;
                const resources::MaterialMeshParams          *_materialParams;
                const resources::Texture2DResourceInterface  *_textureBinds[resources::FG_MATERIAL_TEXTURE_MAX];
                const resources::ShaderResourceInterface     *_shader;
            };

            Model3D();
            ~Model3D() override;

            void  setModelAndMaterial(const fg::string &mdlResourcePath, const fg::string &materialResourcePath) override;
            void  setMaterial(const fg::string &materialResourcePath) override;
            void  setMeshVisible(const fg::string &meshName, bool visible) override;
            void  setMeshAdditionalTransform(const fg::string &meshName, const math::m4x4 &transform) override;

            const math::m4x4 *getMeshTransform(const fg::string &meshName) const override;
            const math::m4x4 *getMeshAdditionalTransform(const fg::string &meshName) const override;
            const math::m4x4 *getHelperTransform(const fg::string &helperName) const override;
            const math::p3d  *getMeshBBoxMinPoint(const fg::string &meshName) const override;
            const math::p3d  *getMeshBBoxMaxPoint(const fg::string &meshName) const override;

            bool  isMeshVisible(const fg::string &meshName) override;

            void  playAnim(const fg::string &animResourcePath, float animLenMs, float animOffsetMs, float smoothTimeMs, AnimationLayer layer = AnimationLayer::LAYER0) override;
            void  setAnimLayerKoeff(AnimationLayer layer, float koeff) const override;
            float getAnimLayerKoeff(AnimationLayer layer) const override;

            void  updateCoordinates(float frameTimeMs) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

            unsigned  getComponentCount() const override;
            RenderObjectInterface::ComponentInterface *getComponentInterface(unsigned index) override;

        protected:
            Animator _animator;

            const resources::ModelResourceInterface     *_model;
            const resources::MaterialResourceInterface  *_material;

            fg::string  _modelResourcePath;
            fg::string  _materialResourcePath;            
            
            unsigned    _meshCount;
            MeshData    *_root;
            MeshData    **_meshes;

            StaticHash  <resources::FG_MESH_MAX, MeshData *> _meshesByName;

            bool _modelReady;
        };
    }
}
