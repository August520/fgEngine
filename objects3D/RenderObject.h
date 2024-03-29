
namespace fg {
    namespace object3d {
        class RenderObject : public RenderObjectInterface {
        public:
            RenderObject();
            RenderObject(render::EngineSceneCompositionInterface *sceneComposition);
            ~RenderObject() override;

            RenderObjectInterface *addChild(RenderObjectInterface *obj) override;
            RenderObjectType getType() const override;

            unsigned removeChild(RenderObjectInterface *obj) override;
            void     removeAllChilds() override;
            unsigned getChildCount() const override;

            RenderObjectInterface *&getParent() override;
            
            void setAddHandler(const callback <void ()> &cb) override;
            void setUpdateHandler(const callback <void (float)> &cb) override;
            void setRemoveHandler(const callback <void ()> &cb) override;

            void callAddHandler() override;
            void callUpdateHandler(float frameTimeMs) override;
            void callRemoveHandler() override;

            void setPosition(float posX, float posY, float posZ) override;
            void setPosition(const math::p3d &position) override;
            void setScaling(float sclX, float sclY, float sclZ) override;
            void setScaling(const math::p3d &scale) override;
            void setRotation(float angleRad, const math::p3d &axis) override;
            void setRotation(const math::quat &rotation) override;
            void setTransform(const math::m4x4 &transform) override;

            void setVisible(bool visible) override;

            void appendPosition(float xInc, float yInc, float zInc) override;
            void appendPosition(const math::p3d &posInc) override;
            void appendRotation(const math::quat &rotInc) override;

            const math::p3d   &getPosition() const override;
            const math::p3d   &getScaling() const override;
            const math::quat  &getRotation() const override;
            const math::m4x4  &getTransform() const override;
            const math::m4x4  &getFullTransform() const override;

            bool  isVisible() const override;

            void  updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;
            
            unsigned  getComponentCount() const override;
            RenderObjectComponentInterface *getComponentInterface(unsigned index) override;

        protected:
            RenderObjectType       _type = RenderObjectType::NONE;
            RenderObjectInterface  *_parent = nullptr;
            
            render::EngineSceneCompositionInterface *_sceneComposition = nullptr;
            std::unordered_set <RenderObjectInterface *> _childs;
                        
            bool         _visible = true;
            math::m4x4   _fullTransform;
            math::m4x4   _localTransform;
            math::p3d    _localPosition;
            math::p3d    _localScale;
            math::quat   _localRotation;

            callback     <void ()> _addHandler;
            callback     <void (float)> _updateHandler;
            callback     <void ()> _removeHandler;

            render::EngineSceneCompositionInterface *getSceneComposition() override;

        private:
            RenderObject(const RenderObject &);
            RenderObject &operator =(const RenderObject &);
        };
    }
}


//