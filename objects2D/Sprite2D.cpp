
namespace fg {
    namespace object2d {
        Sprite2D::Sprite2D() : 
            _clip(nullptr), 
            _clipset(nullptr), 
            _texture(nullptr),
            _timeCounter(0),
            _curFrame(0),
            _frameCounter(0), 
            _frameCount(1), 
            _frameRate(1), 
            _stopped(false)
        {
            _type = DisplayObjectType::SPRITE;
        }

        Sprite2D::~Sprite2D() {
        
        }

        void Sprite2D::setTexture(const fg::string &texResourcePath) {
            _textureResourcePath = texResourcePath;
            _texture = nullptr;
            _frameCount = 1;
            _cycleCallback.set(nullptr);
        }

        void Sprite2D::setClip(const fg::string &clipResourcePath, const fg::string &clipName) {
            _clipResourcePath = clipResourcePath;
            _clipName = clipName;
            _clipset = nullptr;
            _clip = nullptr;
            _frameCount = 1;
            _cycleCallback.set(nullptr);
        }

        float Sprite2D::getWidth() const {
            return _clip ? _clip->width : 0.0f;
        }

        float Sprite2D::getHeight() const {
            return _clip ? _clip->height : 0.0f;
        }

        void Sprite2D::play() {
            _stopped = false;
        }

        void Sprite2D::accelerateToNearSide() {
        
        }

        void Sprite2D::accelerateToForwardSide() {
        
        }

        void Sprite2D::stop() {
            _stopped = true;
        }

        void Sprite2D::gotoAndPlay(unsigned frame) { 
            _curFrame = frame >= _frameCount ? _frameCount - 1 : frame;
            _stopped = false;
        }

        void Sprite2D::gotoAndStop(unsigned frame) {
            _curFrame = frame >= _frameCount ? _frameCount - 1 : frame;
            _stopped = true;
        }

        void Sprite2D::nextFrame() {
            if(++_curFrame == _frameCount) {
                _curFrame = 0;
            }
        }

        void Sprite2D::prevFrame() {
            if(--_curFrame == (unsigned int)(-1)) {
                _curFrame = _frameCount - 1;
            }
        }

        bool Sprite2D::isPlaying() const {
            return !_stopped;
        }

        void Sprite2D::setAnimCycleCallback(const callback <void ()> &cb) {
            _cycleCallback = cb;
        }

        unsigned Sprite2D::getFrameCount() const {
            return _frameCount;
        }

        unsigned Sprite2D::getCurrentFrame() const {
            return _curFrame;
        }

        const resources::Texture2DResourceInterface *Sprite2D::getTexture() const {
            return _texture;
        }

        const resources::ClipData *Sprite2D::getClipData() const {
            return _clip;
        }
        
        bool Sprite2D::hitTestPoint(const math::p2d &point) const {
            return false;
        }

        bool Sprite2D::hitTestLine(const math::p2d &point) const {
            return false;
        }

        void Sprite2D::updateCoordinates(float frameTimeMs) {
            DisplayObject::updateCoordinates(frameTimeMs);

            if(_stopped == false) {
                _timeCounter += unsigned(frameTimeMs);
                unsigned tframe = _timeCounter * _frameRate / 1000;

                if(tframe != _frameCounter) {
                    if(++_curFrame == _frameCount) {
                        _curFrame = 0;

                        if(_cycleCallback.isBinded()) {
                            _cycleCallback();
                        }
                    }

                    _frameCounter = tframe;
                }
            }
        }

        bool Sprite2D::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            if(_texture == nullptr) { 
                if((_texture = resMan.getResource(_textureResourcePath)) == nullptr) {
                    return false;
                }
            }
            if(_clipset == nullptr) { 
                if((_clipset = resMan.getResource(_clipResourcePath)) == nullptr) {
                    return false;
                }
            }
            
            if(_clipset->valid()) {
                if(_clip == nullptr) {
                    if((_clip = _clipset->getClip(_clipName)) != nullptr) {
                        _frameCount = _clip->frameCount;
                        _frameRate = _clip->frameRate;
                    }
                    else return false;
                }

                return _texture->valid();
            }

            _clip = nullptr;
            return false;
        }


    }
}


//