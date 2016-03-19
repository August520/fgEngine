
// TODO: 

namespace fg {
    namespace resources {
        TextureCubeResource::~TextureCubeResource() {
            if(_self) {
                _self->release();
            }
        }

        void TextureCubeResource::loaded(const diag::LogInterface &log) {
            byteinput data(_binaryData, _binarySize);

            if (memcmp(data.getPtr(), "DDS ", 4) == 0) {
                tools::DDS_HEADER ddsh;
                data.readDword();
                data.readBytes((char *)&ddsh, sizeof(tools::DDS_HEADER));

                _sz = ddsh.dwWidth;
                _mipsCount = std::max(ddsh.dwMipMapCount, 1u);
                _format = tools::getDDSFormat(ddsh);

                if (_format != platform::TextureFormat::UNKNOWN && (ddsh.dwCaps2 & tools::DDSF_CUBEMAP) && ddsh.dwWidth == ddsh.dwHeight) {
                    unsigned offset = 0; 
                    
                    for (unsigned c = 0; c < 6; c++) {
                        _imgData[c] = new unsigned char *[_mipsCount];
                    
                        for (unsigned i = 0, tsz = _sz; i < _mipsCount; i++) {
                            _imgData[c][i] = (unsigned char *)(data.getCurrentPtr() + offset);

                            offset += tools::getDDSImageSize(_format, tsz, tsz);
                            tsz >>= 1;
                        }
                    }                    
                }
                else {
                    log.msgError("TextureCubeResource::loaded %s bad format", _loadPath.data());
                    _loadingState = ResourceLoadingState::INVALID;
                }
            }
            else {
                log.msgError("TextureCubeResource::loaded %s bad format", _loadPath.data());
                _loadingState = ResourceLoadingState::INVALID;
            }
        }

        bool TextureCubeResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            if (_loadingState != ResourceLoadingState::INVALID) {
                _self = api.rdCreateTextureCube(_imgData, _sz, _mipsCount, _format);
            }

            for (unsigned c = 0; c < 6; c++) {
                delete _imgData[c];                
            }
            
            return false;
        }

        void TextureCubeResource::unloaded(){
            _self->release();
            _self = nullptr;
        }

        const platform::TextureCubeInterface *TextureCubeResource::getPlatformObject() const {
            return _self;
        }
    }
}

