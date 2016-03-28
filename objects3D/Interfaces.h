
namespace fg {
    namespace render {
        class EngineSceneCompositionInterface;
    }

    namespace object3d {
        enum class RenderObjectType {
            NONE       = 0,
            MODEL      = 1,
            PARTICLES  = 2,
            LIGHT      = 3,
        };

        enum class AnimationLayer {
            LAYER0 = 0,
            LAYER1 = 1,
            LAYER2 = 2,
            LAYER3 = 3,
        };

        class RenderObjectComponentInterface {
        public:
            virtual ~RenderObjectComponentInterface() {}
            virtual bool isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) = 0;
            virtual const math::m4x4 &getFullTransform() const = 0;

        protected: // optimization
            template <bool(*op)(float, float)> friend class RenderObjectComponentCmp;  
            math::m4x4 _fullTransform;              
        };

        class RenderObjectInterface;
        class RenderObjectBase {
        public:
            virtual ~RenderObjectBase() {}
            
            virtual RenderObjectInterface *addChild(RenderObjectInterface *obj) = 0;
            virtual RenderObjectType getType() const = 0;
            
            virtual unsigned removeChild(RenderObjectInterface *obj) = 0;            
            virtual void     removeAllChilds() = 0;            
            virtual unsigned getChildCount() const = 0;
            
            virtual RenderObjectInterface *&getParent() = 0;
            
            virtual void setAddHandler(const callback <void ()> &cb) = 0;
            virtual void setUpdateHandler(const callback <void (float)> &cb) = 0;
            virtual void setRemoveHandler(const callback <void ()> &cb) = 0;
            
            virtual void callAddHandler() = 0;
            virtual void callUpdateHandler(float frameTimeMs) = 0;
            virtual void callRemoveHandler() = 0;

            virtual void setPosition(float posX, float posY, float posZ) = 0;
            virtual void setPosition(const math::p3d &position) = 0;
            virtual void setScaling(float sclX, float sclY, float sclZ) = 0;
            virtual void setScaling(const math::p3d &scale) = 0;
            virtual void setRotation(float angleRad, const math::p3d &axis) = 0;
            virtual void setRotation(const math::quat &rotation) = 0;
            virtual void setTransform(const math::m4x4 &transform) = 0;

            virtual void setVisible(bool visible) = 0;

            virtual void appendPosition(float xInc, float yInc, float zInc) = 0;
            virtual void appendPosition(const math::p3d &posInc) = 0;
            virtual void appendRotation(const math::quat &rotInc) = 0;

            virtual const math::p3d   &getPosition() const = 0;
            virtual const math::p3d   &getScaling() const = 0;
            virtual const math::quat  &getRotation() const = 0;
            virtual const math::m4x4  &getTransform() const = 0;
            virtual const math::m4x4  &getFullTransform() const = 0;
            
            virtual bool  isVisible() const = 0;
                        
            virtual void  updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) = 0;
            virtual bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) = 0;

            virtual unsigned  getComponentCount() const = 0;
            virtual RenderObjectComponentInterface *getComponentInterface(unsigned index) = 0;
            virtual render::EngineSceneCompositionInterface *getSceneComposition() = 0;
        };

        class RenderObjectInterface : virtual public RenderObjectBase {
        public:
            virtual ~RenderObjectInterface() {}
        };

        //---

        class Model3DBase {
        public:
            class MeshComponentInterface : public RenderObjectComponentInterface {
            public:
                virtual ~MeshComponentInterface() {}
                
                virtual unsigned getSkinMatrixCount() const = 0;
                virtual unsigned getTextureBindCount() const = 0;

                virtual const math::p3d &getMaterialMetalness() const = 0;
                virtual float getMaterialGlossiness() const = 0;
                
                virtual const math::m4x4 *getSkinMatrixArray() const = 0;
                virtual const resources::Texture2DResourceInterface *getTextureBind(unsigned bindIndex) const = 0;
                virtual const resources::ShaderResourceInterface *getShader() const = 0;
                virtual const resources::MeshInterface *getMesh() const = 0;
                
                virtual bool isTransparent() const = 0;
                virtual bool isVisible() const = 0;
                virtual bool isSkinned() const = 0;
                virtual bool isShadowCaster() const = 0;
                virtual bool isBackFaced() const = 0;
            };

            virtual ~Model3DBase() {}

            virtual void  setColor(float r, float g, float b, float a) = 0;
            virtual void  setColor(const fg::color &rgba) = 0;

            virtual void  setModelAndMaterial(const fg::string &mdlResourcePath, const fg::string &materialResourcePath) = 0;
            virtual void  setMaterial(const fg::string &materialResourcePath) = 0;
            virtual void  setMeshVisible(const fg::string &meshName, bool visible) = 0;
            virtual void  setHierarchyVisible(const fg::string &rootMeshName, bool visible) = 0;
            virtual void  setMeshAdditionalTransform(const fg::string &meshName, const math::m4x4 &transform) = 0;
            virtual void  setUseAnimInterpolation(bool use) = 0;

            virtual const math::m4x4 *getMeshTransform(const fg::string &meshName) const = 0;
            virtual const math::m4x4 *getMeshAdditionalTransform(const fg::string &meshName) const = 0;
            virtual const math::m4x4 *getHelperTransform(const fg::string &helperName) const = 0;
            virtual const math::p3d  *getMeshBBoxMinPoint(const fg::string &meshName) const = 0;
            virtual const math::p3d  *getMeshBBoxMaxPoint(const fg::string &meshName) const = 0;

            virtual void  getHelpers(std::vector <std::string> &helpers) const = 0;

            virtual bool  isMeshVisible(const fg::string &meshName) = 0;
            virtual const fg::color &getColor() const = 0;

            virtual void  playAnim(const fg::string &animResourcePath, float animLenMs, float animOffsetMs, float smoothTimeMs, bool cycled, AnimationLayer layer = AnimationLayer::LAYER0) = 0;
            virtual void  setAnimFinishCallback(const callback <void()> &cb, AnimationLayer layer = AnimationLayer::LAYER0) = 0;
            virtual void  setAnimLayerTimeScale(float scale, AnimationLayer layer = AnimationLayer::LAYER0) = 0;
            virtual void  setAnimLayerKoeff(float koeff, AnimationLayer layer = AnimationLayer::LAYER0) = 0;
            virtual float getAnimLayerKoeff(AnimationLayer layer = AnimationLayer::LAYER0) const = 0;
        };

        class Model3DInterface : public RenderObjectInterface, virtual public Model3DBase {
        public:
            virtual ~Model3DInterface() {}
        };

        //---
        
        class Particles3DBase {
        public:
            class EmitterComponentInterface : public RenderObjectComponentInterface {
            public:
                virtual ~EmitterComponentInterface() {}
                virtual unsigned getTextureBindCount() const = 0;

                virtual const particles::EmitterInterface *getEmitter() const = 0;
                virtual const resources::ShaderResourceInterface *getShader() const = 0;
                virtual const resources::Texture2DResourceInterface *getTextureBind(unsigned bindIndex) const = 0;

                virtual const math::m4x4 &getTransformHistory(float timeBeforeMs) const = 0;
            };

            virtual ~Particles3DBase() {}
            virtual void  setResource(const fg::string &particlesResourcePath) = 0;            
            virtual const particles::EmitterInterface *getEmitter(const fg::string &name) const = 0;
        };

        class Particles3DInterface : public RenderObjectInterface, virtual public Particles3DBase {
        public:
            virtual ~Particles3DInterface() {}
        };

        //---

        class PointLightBase {
        public:
            virtual ~PointLightBase() {}

            virtual void  setDistance(float dist) = 0;
            virtual void  setColor(float r, float g, float b, float a) = 0;
            virtual void  setColor(const fg::color &rgba) = 0;

            virtual float getDistance() const = 0;
            virtual const fg::color &getColor() const = 0;
        };

        class PointLightInterface : public RenderObjectInterface, virtual public PointLightBase {
        public:
            virtual ~PointLightInterface() {}
        };
    }
}



//