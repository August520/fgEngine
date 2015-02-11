
namespace fg {
    namespace render {
        template <typename BASE> class ShaderConstantBufferStruct {
        public:
            BASE data;

            ShaderConstantBufferStruct(platform::PlatformInterface &iplatform, platform::ShaderConstBufferUsing use);
            virtual ~ShaderConstantBufferStruct();

            void update();
            void apply();
            void updateAndApply();
            void release();

        protected:
            platform::PlatformInterface &_platform;
            platform::ShaderConstantBufferInterface *_self;

        private:
            ShaderConstantBufferStruct(const ShaderConstantBufferStruct <BASE> &);
            ShaderConstantBufferStruct <BASE> &operator =(const ShaderConstantBufferStruct <BASE> &);
        };

        //---

        template <typename BASE> ShaderConstantBufferStruct <BASE>::ShaderConstantBufferStruct(platform::PlatformInterface &iplatform, platform::ShaderConstBufferUsing use)
            : _platform(iplatform) {
            _self = _platform.rdCreateShaderConstantBuffer(use, sizeof(BASE));
        }

        template <typename BASE> ShaderConstantBufferStruct <BASE>::~ShaderConstantBufferStruct() {
            _self->release();
        }

        template <typename BASE> void ShaderConstantBufferStruct <BASE>::update() {
            _self->update(&data);
        }

        template <typename BASE> void ShaderConstantBufferStruct <BASE>::apply() {
            _platform.rdSetShaderConstBuffer(_self);
        }

        template <typename BASE> void ShaderConstantBufferStruct <BASE>::updateAndApply() {
            _self->update(&data);
            _platform.rdSetShaderConstBuffer(_self);
        }

        template <typename BASE> void ShaderConstantBufferStruct <BASE>::release() {
            _self->release();
            _self = nullptr;
        }
    }
}


