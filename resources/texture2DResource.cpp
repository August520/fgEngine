
// TODO: 

namespace fg {
    namespace resources {
        Texture2DResource::~Texture2DResource() {
            if(_self) {
                _self->release();
            }
        }

        void Texture2DResource::loaded(const diag::LogInterface &log) {
            byteinput data(_binaryData, _binarySize);

            if(*(unsigned *)data.getPtr() == 0xf0fa5566) {
                data.readDword();
                _format = platform::TextureFormat::RGBA8;

                unsigned char type = data.readByte();
                unsigned int  flags = data.readDword();
                _szx = data.readDword();
                _szy = data.readDword();
                _mipsCount = data.readDword();

                _imgDataIsDynamic = true;
                _imgData = new unsigned char *[_mipsCount];

                for(unsigned i = 0; i < _mipsCount; i++) {
                    unsigned tw = 0, th = 0;
                    unsigned mipDataSize = data.readDword();

                    tools::lodepng_decode32(&_imgData[i], &tw, &th, (unsigned char *)(data.getPtr() + data.getOffset()), mipDataSize);
                    data.startOff(data.getOffset() + mipDataSize);

                    if(i == 0) {
                        _szx = tw;
                        _szy = th;
                    }
                }
            }
            else if (memcmp(data.getPtr(), "DDS ", 4) == 0) {
                tools::DDS_HEADER ddsh;
                data.readDword();
                data.readBytes((char *)&ddsh, sizeof(tools::DDS_HEADER));

                _szx = ddsh.dwWidth;
                _szy = ddsh.dwHeight;
                _mipsCount = ddsh.dwMipMapCount;
                _format = tools::getDDSFormat(ddsh);

                if (_format != platform::TextureFormat::UNKNOWN && (ddsh.dwCaps2 & tools::DDSF_CUBEMAP) == 0 && (ddsh.dwCaps2 & tools::DDSF_VOLUME) == 0) {
                    _imgData = new unsigned char *[_mipsCount];
                    
                    unsigned tw = _szx;
                    unsigned th = _szy;
                    unsigned mipOffset = 0; 

                    for (unsigned i = 0; i < _mipsCount; i++) {
                        _imgData[i] = (unsigned char *)(data.getCurrentPtr() + mipOffset);

                        tw >>= 1;
                        th >>= 1;

                        mipOffset += tools::getDDSImageSize(_format, tw, th);
                    }
                }
                else {
                    log.msgError("ResTexture::_loadedCallback %s bad format", _loadPath.data());
                    _loadingState = ResourceLoadingState::INVALID;
                }
            }
            else {
                _imgData = new unsigned char *[1];
                _imgDataIsDynamic = true;

                if(!tools::lodepng_decode32(&_imgData[0], &_szx, &_szy, (unsigned char *)(data.getPtr() + data.getOffset()), data.getSize())) {
                    _format = platform::TextureFormat::RGBA8;
                    _mipsCount = 1;
                }
                else {
                    log.msgError("ResTexture::_loadedCallback %s bad format", _loadPath.data());
                    _loadingState = ResourceLoadingState::INVALID;
                }
            }
        }

        bool Texture2DResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            if (_loadingState != ResourceLoadingState::INVALID) {
                _self = api.rdCreateTexture2D(_imgData, _szx, _szy, _mipsCount, _format);

                if (_imgDataIsDynamic) {
                    for (unsigned i = 0; i < _mipsCount; i++) {
                        free(_imgData[i]);
                    }
                }

                delete[] _imgData;
                _imgData = nullptr;
            }
            
            return false;
        }

        void Texture2DResource::unloaded(){
            _self->release();
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

