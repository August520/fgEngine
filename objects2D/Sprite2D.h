
namespace fg {
    namespace object2d {
        class Sprite2D : public Sprite2DInterface, public DisplayObject {
        public:
            Sprite2D();
            ~Sprite2D() override;

            void  setTexture(const fg::string &texResourcePath) override;
            void  setClip(const fg::string &clipResourcePath, const fg::string &clipName) override;

            float getWidth() const override;
            float getHeight() const override;

            void  play() override;
            void  accelerateToNearSide() override;
            void  accelerateToForwardSide() override;
            void  stop() override;
            void  gotoAndPlay(unsigned frame) override;
            void  gotoAndStop(unsigned frame) override;
            void  nextFrame() override;
            void  prevFrame() override;

            bool  isPlaying() const override;
            void  setAnimCycleCallback(const callback <void ()> &cb) override;

            unsigned getFrameCount() const override;
            unsigned getCurrentFrame() const override;

            const resources::Texture2DResourceInterface *getTexture() const override;
            const resources::ClipData *getClipData() const override;

            bool  hitTestPoint(const math::p2d &point) const override;
            bool  hitTestLine(const math::p2d &point) const override;

            void  updateCoordinates(float frameTimeMs) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

        protected:
            fg::string  _textureResourcePath;
            fg::string  _clipResourcePath;
            fg::string  _clipName;

            unsigned    _timeCounter;
            unsigned    _frameRate;
            unsigned    _frameCounter;
            unsigned    _frameCount;
            int         _curFrame;
            bool        _stopped;

            const resources::Texture2DResourceInterface  *_texture;
            const resources::ClipSetResourceInterface    *_clipset;
            const resources::ClipData *_clip;

            callback <void ()> _cycleCallback;
        };
    }
}
