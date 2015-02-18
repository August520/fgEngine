
// TODO: 

namespace fg {
    namespace resources {
        Texture2DResource::~Texture2DResource() {
            if(_self) {
                _self->release();
            }
        }

        void Texture2DResource::loaded(const diag::LogInterface &log) {
            byteform data (_binaryData, _binarySize);

            if(*(unsigned *)data.getPtr() == 0xf0fa5566) {
                data.readDword();

                unsigned char type = data.readByte();
                unsigned int  flags = data.readDword();
                _szx = data.readDword();
                _szy = data.readDword();
                _mipsCount = data.readDword();

                _imgData = new unsigned char *[_mipsCount];

                for(unsigned i = 0; i < _mipsCount; i++) {
                    unsigned tw = 0, th = 0;
                    unsigned mipDataSize = data.readDword();

                    tools::lodepng_decode32(&_imgData[i], &tw, &th, (unsigned char *)(data.getPtr() + data.getOffset()), mipDataSize);
                    data.incOffset(mipDataSize);

                    if(i == 0) {
                        _szx = tw;
                        _szy = th;
                    }
                }
            }
            else {
                _imgData = new unsigned char *[1];

                if(!tools::lodepng_decode32(&_imgData[0], &_szx, &_szy, (unsigned char *)(data.getPtr() + data.getOffset()), data.getSize())) {
                    unsigned char cR = _imgData[0][0];
                    unsigned char cG = _imgData[0][1];
                    unsigned char cB = _imgData[0][2];
                    unsigned char cA = _imgData[0][3];

                    _mipsCount = 1;
                }
                else {
                    log.msgError("ResTexture::_loadedCallback %s bad format", _loadPath);
                    _loadingState = ResourceLoadingState::INVALID;
                }
            }
        }

        bool Texture2DResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            _self = api.rdCreateTexture2D(_imgData, _szx, _szy, _mipsCount);

            for(unsigned i = 0; i < _mipsCount; i++) {
                free(_imgData[i]);
            }

            delete[] _imgData;
            _imgData = nullptr;

            return false;
        }

        void Texture2DResource::unloaded(){ 
            delete _self;
            _self = nullptr;
        }

        unsigned Texture2DResource::getWidth() const{
            return _szx;
        }

        unsigned Texture2DResource::getHeight() const{
            return _szy;
        }

        const platform::Texture2DInterface *Texture2DResource::getPlatformObject() const {
            return _self;
        }
    }
}

