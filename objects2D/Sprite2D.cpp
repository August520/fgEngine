
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
            _frameCounter = 0;
            _timeCounter = 0;
            _cycleCallback.set(nullptr);
        }

        void Sprite2D::setClip(const fg::string &clipResourcePath, const fg::string &clipName) {
            _clipResourcePath = clipResourcePath;
            _clipName = clipName;
            _clipset = nullptr;
            _clip = nullptr;
            _frameCounter = 0;
            _timeCounter = 0;
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
            _curFrame = frame;
            _stopped = false;
        }

        void Sprite2D::gotoAndStop(unsigned frame) {
            _curFrame = frame;
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
        
        bool Sprite2D::hitTestPoint(const math::p2d &point, const math::p2d &dpiFactor) const {
            math::m3x3 invFullTransform;
            math::p2d  tp = point;
            math::p2d  dpi = _resolutionDependent ? dpiFactor : math::p2d(1, 1);

            invFullTransform.inverse(_fullTransform);
            tp.transform(invFullTransform, true);

            if(_clip) {
                tp.x += _clip->centerX * dpi.x;
                tp.y += _clip->centerY * dpi.y;

                if(_clip->boundingCoords) {
                    math::p2d rightInf(tp.x + 10000.0f, tp.y);
                    int intersects = 0;

                    for(unsigned int i = 0; i < _clip->boundingCount - 1; i++) {
                        math::p2d p0 (_clip->boundingCoords[i].x * dpi.x, _clip->boundingCoords[i].y * dpi.y);
                        math::p2d p1 (_clip->boundingCoords[i + 1].x * dpi.x, _clip->boundingCoords[i + 1].y * dpi.y);

                        float dpx = p1.x - p0.x;
                        float dpy = p1.y - p0.y;

                        float z2 = (tp.x - p0.x) * dpy - (tp.y - p0.y) * dpx;
                        float z3 = (rightInf.x - p0.x) * dpy - (rightInf.y - p0.y) * dpx;

                        if((tp.y - p0.y) * (tp.y - p1.y) < 0 && z2 * z3 < 0) {
                            intersects++;
                        }
                    }

                    if(intersects & 0x1) return true;
                }
                else {
                    if(tp.x >= 0 && tp.x <= _clip->width * dpi.x) {
                        if(tp.y >= 0 && tp.y <= _clip->height * dpi.y) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        bool Sprite2D::hitTestLine(const math::p2d &point, const math::p2d &dpiFactor) const {
            return false;
        }

        void Sprite2D::updateCoordinates(float frameTimeMs) {
            DisplayObject::updateCoordinates(frameTimeMs);

            if(_stopped == false && _clip) {
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