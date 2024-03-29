
namespace fg {
    namespace object2d {
        class DisplayObject : public DisplayObjectInterface {
        public:
            DisplayObject();
            DisplayObject(render::EngineSceneCompositionInterface *sceneComposition);
            ~DisplayObject() override;

            DisplayObjectInterface *addChild(DisplayObjectInterface *obj) override;
            DisplayObjectType getType() const override;

            unsigned removeChild(DisplayObjectInterface *obj) override;
            void     removeAllChilds() override;
            unsigned getChildCount() const override;

            DisplayObjectInterface *&getParent() override;
            
            void  setAddHandler(const callback <void ()> &cb) override;
            void  setUpdateHandler(const callback <void (float)> &cb) override;
            void  setRemoveHandler(const callback <void ()> &cb) override;

            void  callAddHandler() override;
            void  callUpdateHandler(float frameTimeMs) override;
            void  callRemoveHandler() override;
            
            void  setPosition(float x, float y) override;
            void  setPosition(const math::p2d &pos) override;
            void  setRotation(float degrees) override;
            void  setRotation(const math::p2d &dir) override;
            void  setScaling(float sclx, float scly) override;
            void  setScaling(const math::p2d &scale) override;

            void  appendPosition(float x, float y) override;
            void  appendPosition(const math::p2d &posIncrement) override;
            void  appendRotation(float degrees) override;

            void  setTransform(const math::m3x3 &trasformMatrix) override;
            void  setVisible(bool visible) override;
            void  setResolutionDependency(bool value) override;

            const math::m3x3 &getFullTransform() const override;
            const math::m3x3 &getLocalTransform() const override;
            const math::p2d  &getPosition() const override;
            const math::p2d  &getScaling() const override;

            void  setZ(float z) override;
            float getZ() const override;

            void  setAlpha(float alpha) override;
            float getAlpha() const override;
            float getRotation() const override;

            bool  isResolutionDepended() const override;
            bool  isVisible() const override;
            bool  hitTestPoint(const math::p2d &point, const math::p2d &dpiFactor) const override;
            bool  hitTestLine(const math::p2d &point, const math::p2d &dpiFactor) const override;

            void  updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;
            
        protected:
            DisplayObjectType _type = DisplayObjectType::NONE;
            DisplayObjectInterface *_parent = nullptr;

            render::EngineSceneCompositionInterface *_sceneComposition = nullptr;
            std::unordered_set <DisplayObjectInterface *> _childs;
            
            math::m3x3   _fullTransform;
            math::m3x3   _localTransform;
            math::p2d    _localScale;
            float        _rotationInDegrees = 0.0f;
            float        _zCoord = 0.0f;
            float        _alpha = 1.0f;
            bool         _visible = true;
            bool         _resolutionDependent = false;

            callback     <void ()> _addHandler;
            callback     <void (float)> _updateHandler;
            callback     <void ()> _removeHandler;

            render::EngineSceneCompositionInterface *getSceneComposition() override;

        private:
            DisplayObject(const DisplayObject &);
            DisplayObject &operator =(const DisplayObject &);
        };


    }
}