
#include <algorithm>

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
            void playAnim(const fg::string &animResourcePath, float animLenMs, float animOffsetMs, float smoothTimeMs, bool cycled, AnimationLayer ilayer);
            void setAnimFinishCallback(const callback <void()> &cb, AnimationLayer ilayer);
            void setUseInterpolation(bool useInterpolation);
            void setTimeScale(float scale);
            
            bool isDirtyResources() const;

            unsigned getActiveLayersCount() const;
            float    getAnimKoeff(AnimationLayer ilayer) const;

        private:
            struct Layer {
                const resources::AnimationResourceInterface *curAnimation = nullptr;
                const resources::AnimationResourceInterface *nextAnimation = nullptr;
            
                fg::string  curAnimResourcePath;
                fg::string  nextAnimResourcePath;
                
                float curAnimTimeLen = 0.0f;
                float curAnimTimePass = 0.0f;
                bool  curAnimCycled = true;
                float nextAnimTimeLen = 0.0f;
                float nextAnimTimePass = 0.0f;
                float nextAnimTimeOffset = 0.0f;
                bool  nextAnimCycled = true;
                float smoothTime = 0.0f;
                
                callback  <void()> animFinishCallback;

                Layer() {}
            };

            Layer     _layers[FG_ANIM_LAYERS_MAX];
            unsigned  _activeLayers = 0;
            float     _scale = 1.0f;
            bool      _isDirty = false;

            bool (resources::AnimationResourceInterface::*_getTransformMethod)(const fg::string &boneName, float animKoeff, bool cycled, math::p3d &oTranslation, math::quat &oRotation, math::p3d &oScaling) const;
            
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

                const math::p3d &getMaterialMetalness() const override;
                float getMaterialGlossiness() const override;

                const resources::Texture2DResourceInterface *getTextureBind(unsigned bindIndex) const override;
                const resources::ShaderResourceInterface *getShader() const override;
                const resources::MeshInterface *getMesh() const override;

                bool isTransparent() const override;
                bool isSkinned() const override;
                bool isVisible() const override;

                bool isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

            protected:
                bool      _skinned = false;
                bool      _visible = false;
                MeshData  **_childs = nullptr;
                
                unsigned  _childCount = 0;
                unsigned  _skinMatrixCount = 0;

                math::m4x4  _additionalTransform;                
                math::m4x4  *_skinMatrixes = nullptr;

                bool      _transformApplied = false;
                bool      _visibilityApplied = false;

                const resources::MeshInterface               *_mesh = nullptr;
                const resources::MaterialMeshParams          *_materialParams = nullptr;
                const resources::Texture2DResourceInterface  *_textureBinds[resources::FG_MATERIAL_TEXTURE_MAX];
                const resources::ShaderResourceInterface     *_shader = nullptr;
            };

            Model3D();
            ~Model3D() override;

            void  setColor(float r, float g, float b, float a) override;
            void  setColor(const fg::color &rgba) override;

            void  setModelAndMaterial(const fg::string &mdlResourcePath, const fg::string &materialResourcePath) override;
            void  setMaterial(const fg::string &materialResourcePath) override;
            void  setMeshVisible(const fg::string &meshName, bool visible) override;
            void  setHierarchyVisible(const fg::string &rootMeshName, bool visible) override;
            void  setMeshAdditionalTransform(const fg::string &meshName, const math::m4x4 &transform) override;
            void  setUseAnimInterpolation(bool use);

            const math::m4x4 *getMeshTransform(const fg::string &meshName) const override;
            const math::m4x4 *getMeshAdditionalTransform(const fg::string &meshName) const override;
            const math::m4x4 *getHelperTransform(const fg::string &helperName) const override;
            const math::p3d  *getMeshBBoxMinPoint(const fg::string &meshName) const override;
            const math::p3d  *getMeshBBoxMaxPoint(const fg::string &meshName) const override;

            const fg::color &getColor() const override;
            bool  isMeshVisible(const fg::string &meshName) override;

            void  playAnim(const fg::string &animResourcePath, float animLenMs, float animOffsetMs, float smoothTimeMs, bool cycled, AnimationLayer layer = AnimationLayer::LAYER0) override;
            void  setAnimFinishCallback(const callback <void()> &cb, AnimationLayer layer = AnimationLayer::LAYER0) override;
            void  setAnimLayerTimeScale(float scale, AnimationLayer layer = AnimationLayer::LAYER0) override;
            void  setAnimLayerKoeff(float koeff, AnimationLayer layer = AnimationLayer::LAYER0) override;
            float getAnimLayerKoeff(AnimationLayer layer) const override;

            void  updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;
            
            unsigned  getComponentCount() const override;
            RenderObjectComponentInterface *getComponentInterface(unsigned index) override;

        protected:
            Animator _animator;

            const resources::ModelResourceInterface     *_model;
            const resources::MaterialResourceInterface  *_material;

            fg::string  _modelResourcePath;
            fg::string  _materialResourcePath;
            unsigned    _meshCount;
            MeshData    *_root;
            MeshData    **_meshes;
            bool        _modelReady;
            fg::color   _rgba;

            mutable StaticHash  <resources::FG_MESH_MAX, MeshData *> _meshesByName;
            std::vector         <callback <void()>> _resourceReadyApplies;

            MeshData *_getOrCreateMeshByName(const fg::string &meshName) const;
        };
    }
}
