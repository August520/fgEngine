
namespace fg {
    namespace object2d {
        enum class DisplayObjectType {
            NONE       = 0,
            SPRITE     = 1,
            TEXTFIELD  = 2,
            GEOMETRY   = 3,
        };

        class DisplayObjectInterface;
        
        class DisplayObjectBase {
        public:
            virtual ~DisplayObjectBase() {}

            virtual DisplayObjectInterface *addChild(DisplayObjectInterface *obj) = 0;
            virtual DisplayObjectType getType() const = 0;
            
            virtual unsigned removeChild(DisplayObjectInterface *obj) = 0;
            virtual unsigned removeChild(unsigned index) = 0;
            virtual void     removeAllChilds() = 0;            
            virtual unsigned getChildCount() const = 0;
                        
            virtual DisplayObjectInterface *getChildAt(unsigned index) const = 0;
            virtual DisplayObjectInterface *getChildBack() const = 0;
            virtual DisplayObjectInterface *&getParent() = 0;
            virtual DisplayObjectInterface *&getNext() = 0;
            virtual DisplayObjectInterface *&getBack() = 0;
            
            virtual void  setAddHandler(const callback <void ()> &cb) = 0;
            virtual void  setUpdateHandler(const callback <void (float)> &cb) = 0;
            virtual void  setRemoveHandler(const callback <void ()> &cb) = 0;
            
            virtual void  callAddHandler() = 0;
            virtual void  callUpdateHandler(float frameTimeMs) = 0;
            virtual void  callRemoveHandler() = 0;
            
            virtual void  setPosition(float x, float y) = 0;
            virtual void  setPosition(const math::p2d &pos) = 0;
            virtual void  setRotation(float degrees) = 0;
            virtual void  setRotation(const math::p2d &dir) = 0;
            virtual void  setScaling(float sclx, float scly) = 0;
            virtual void  setScaling(const math::p2d &scale) = 0;

            virtual void  appendPosition(float x, float y) = 0;
            virtual void  appendPosition(const math::p2d &posIncrement) = 0;
            virtual void  appendRotation(float degrees) = 0;

            virtual void  setTransform(const math::m3x3 &trasformMatrix) = 0;
            virtual void  setVisible(bool visible) = 0;
            virtual void  setResolutionDependency(bool value) = 0;

            virtual const math::m3x3 &getFullTransform() const = 0;
            virtual const math::m3x3 &getLocalTransform() const = 0;
            virtual const math::p2d  &getPosition() const = 0;
            virtual const math::p2d  &getScaling() const = 0;

            virtual void  setZ(float z) = 0;
            virtual float getZ() const = 0;
            
            virtual void  setAlpha(float alpha) = 0;
            virtual float getAlpha() const = 0;
            virtual float getRotation() const = 0;

            virtual bool  isResolutionDepended() const = 0;
            virtual bool  isVisible() const = 0;
            virtual bool  hitTestPoint(const math::p2d &point, const math::p2d &dpiFactor) const = 0;
            virtual bool  hitTestLine(const math::p2d &point, const math::p2d &dpiFactor) const = 0;
            
            virtual void  updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) = 0;
            virtual bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) = 0;
        };

        class DisplayObjectInterface : virtual public DisplayObjectBase {
        public:
            virtual ~DisplayObjectInterface() {}
        };

        //---

        class Sprite2DBase {
        public:
            virtual ~Sprite2DBase() {}

            virtual void  setTexture(const fg::string &texResourceName) = 0;
            virtual void  setClip(const fg::string &clipResourceName, const fg::string &clipName) = 0;

            virtual float getWidth() const = 0;
            virtual float getHeight() const = 0;
            
            virtual void  play() = 0;
            virtual void  accelerateToNearSide() = 0;
            virtual void  accelerateToForwardSide() = 0;
            virtual void  stop() = 0;
            virtual void  gotoAndPlay(unsigned frame) = 0;
            virtual void  gotoAndStop(unsigned frame) = 0;
            virtual void  nextFrame() = 0;
            virtual void  prevFrame() = 0;

            virtual bool  isPlaying() const = 0;
            virtual void  setAnimCycleCallback(const callback <void ()> &cb) = 0;
            virtual void  setScissorRect(const math::p2d &lt, const math::p2d &rb) = 0;

            virtual unsigned getFrameCount() const = 0;
            virtual unsigned getCurrentFrame() const = 0;

            virtual const resources::Texture2DResourceInterface *getTexture() const = 0;
            virtual const resources::ClipData *getClipData() const = 0;

            virtual const math::p2d &getScissorRectLT() const = 0;
            virtual const math::p2d &getScissorRectRB() const = 0;
        };

        class Sprite2DInterface : public DisplayObjectInterface, virtual public Sprite2DBase {
        public:
            virtual ~Sprite2DInterface() {}
        };

        //---

        enum class TextAlign {
            LEFT,
            CENTER,
            RIGHT,
        };

        struct FontForm {
            fg::color rgba;
            unsigned  size = 20;
            unsigned  glow = 0;
            int shadowX = 0;
            int shadowY = 0;
        };

        class TextFieldBase {
        public:
            virtual ~TextFieldBase() {}

            virtual void  appendText(const std::string &text) = 0;
            virtual void  setText(const std::string &text) = 0;
            virtual void  setFont(const fg::string &fontResourceName) = 0;
            virtual void  setSize(unsigned size) = 0;
            virtual void  setColor(const fg::color &c) = 0;
            virtual void  setGlow(unsigned pixels) = 0;
            virtual void  setShadow(int shadowX, int shadowY) = 0;
            virtual void  setAlign(TextAlign align) = 0;

            virtual const resources::FontResourceInterface *getFont() const = 0;
            virtual const std::string &getText() const = 0;
            virtual const FontForm &getForm() const = 0;
            virtual TextAlign getAlign() const = 0;
        };

        class TextFieldInterface : public DisplayObjectInterface, virtual public TextFieldBase {
        public:
            virtual ~TextFieldInterface() {}
        };
        //---

        class DisplayObjectIteratorInterface;

    }
}



//