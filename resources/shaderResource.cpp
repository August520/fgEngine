
namespace fg {
    namespace resources {
        ShaderResource::~ShaderResource() {
            if(_self) {
                _self->release();
            }
        }

        void ShaderResource::loaded(const diag::LogInterface &log) {

        }

        bool ShaderResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            if((_self = api.rdCreateShader(byteinput(_binaryData, _binarySize))) == nullptr) {
                log.msgError("[ResVertexShader::constructed] can't create vertex shader");
            }
            return false;
        }

        void ShaderResource::unloaded() {
            if(_self) {
                _self->release();
                _self = nullptr;
            }
        }

        const platform::ShaderInterface *ShaderResource::getPlatformObject() const {
            return _self;
        }
    }
}

