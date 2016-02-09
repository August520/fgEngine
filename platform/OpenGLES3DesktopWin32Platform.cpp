
namespace fg {
    namespace opengl {
        const unsigned __VERTEX_PARAMS_MAX        = 4;
        const unsigned __INSTANCE_PARAMS_MAX      = 2;
        const unsigned __UBO_NAMES_MAX            = 4;
        const unsigned __VERTEX_COMPONENT_MAX     = 7;
        const unsigned __INSTANCE_COMPONENT_START = 8;
        const unsigned __INSTANCE_COMPONENT_MAX   = 8;
        const unsigned __BUFFER_MAX               = 4096;
        
        struct VertexParams {
            unsigned size;
            unsigned layoutCount;
            unsigned floatCounts[16];
        } 
        __vertexParams[__VERTEX_PARAMS_MAX] = {
            {3 * sizeof(float), 1, {3}},
            {5 * sizeof(float), 2, {3, 2}},
            {14 * sizeof(float), 5, {3, 2, 3, 3, 3}},
            {22 * sizeof(float), 7, {3, 2, 3, 3, 3, 4, 4}},
        };

        struct InstanceParams {
            unsigned size;
            unsigned layoutCount;
            unsigned floatCounts[16];
        }
        __instanceParams[__INSTANCE_PARAMS_MAX] = {
            {20 * sizeof(float), 5, {4, 4, 4, 4, 4}},
            {8 * sizeof(float), 2, {4, 4}},
        };

        const char *__uboNames[__UBO_NAMES_MAX] = {
            "FrameData",
            "SkinData",
            "MaterialData",
            "AdditionalData",
        };

        const char *__vertexComp[__VERTEX_COMPONENT_MAX] = {
            "position",
            "texcoord",
            "normal",
            "binormal",
            "tangent",
            "bIndexes",
            "bWeights",
        };

        const char *__instanceComp[__INSTANCE_COMPONENT_MAX] = {
            "instance_data0",
            "instance_data1",
            "instance_data2",
            "instance_data3",
            "instance_data4",
            "instance_data5",
            "instance_data6",
            "instance_data7",
        };

        const char *__textureSamplerNames[platform::TEXTURE_UNITS_MAX] = {
            "texture0",
            "texture1",
            "texture2",
            "texture3",
            "texture4",
            "texture5",
            "texture6",
            "texture7",
        };

        char   __buffer[__BUFFER_MAX];
        GLenum __nativeTextureFormat[] = {GL_RGBA, GL_RGBA, GL_RED, GL_NONE, GL_NONE, GL_NONE};
        GLenum __nativeTextureInternalFormat[] = {GL_RGBA8, GL_RGBA8, GL_R8, GL_NONE, GL_NONE, GL_NONE};
        GLenum __nativeTopology[] = {GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP};
        GLenum __nativeCmpFunc[] = {GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS};

        //---

        ES3DesktopWin32VertexBuffer::ES3DesktopWin32VertexBuffer(platform::VertexType type, unsigned vcount, GLenum usage) {
            unsigned index = unsigned(type);
            unsigned offset = 0;
            
            _vao = 0;
            _vbo = 0;
            _usage = usage;
            _attribsCount = __vertexParams[index].layoutCount;

            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);       
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            
            for(unsigned i = 0; i < _attribsCount; i++) {
                glVertexAttribPointer(i, __vertexParams[index].floatCounts[i], GL_FLOAT, GL_FALSE, __vertexParams[index].size, (const GLvoid *)offset);
                glEnableVertexAttribArray(i);

                offset += __vertexParams[index].floatCounts[i] * sizeof(float);
            }

            _length = offset * vcount;
            glBufferData(GL_ARRAY_BUFFER, _length, nullptr, _usage);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        ES3DesktopWin32VertexBuffer::~ES3DesktopWin32VertexBuffer() {

        }

        GLuint ES3DesktopWin32VertexBuffer::getVAO() const {
            return _vao;
        }

        void *ES3DesktopWin32VertexBuffer::lock() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            auto ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, _length, GL_MAP_WRITE_BIT);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return ptr;
        }

        void ES3DesktopWin32VertexBuffer::unlock() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
       
        void ES3DesktopWin32VertexBuffer::release() {
            glDeleteBuffers(1, &_vbo);
            glDeleteVertexArrays(1, &_vao);
            delete this;
        }

        unsigned ES3DesktopWin32VertexBuffer::getLength() const {
            return _length;
        }

        //---

        ES3DesktopWin32IndexedVertexBuffer::ES3DesktopWin32IndexedVertexBuffer(platform::VertexType type, unsigned vcount, unsigned icount, GLenum usage) {
            unsigned index = unsigned(type);
            unsigned offset = 0;

            _vao = 0;
            _ibo = 0;
            _vbo = 0;
            _usage = usage;
            _attribsCount = __vertexParams[index].layoutCount;

            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);
            
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glGenBuffers(1, &_ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

            for(unsigned i = 0; i < _attribsCount; i++) {
                glVertexAttribPointer(i, __vertexParams[index].floatCounts[i], GL_FLOAT, GL_FALSE, __vertexParams[index].size, (const GLvoid *)offset);
                glEnableVertexAttribArray(i);

                offset += __vertexParams[index].floatCounts[i] * sizeof(float);
            }

            _vlength = offset * vcount;
            _ilength = icount * sizeof(unsigned short);

            glBufferData(GL_ARRAY_BUFFER, _vlength, nullptr, _usage);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _ilength, nullptr, _usage);
            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        ES3DesktopWin32IndexedVertexBuffer::~ES3DesktopWin32IndexedVertexBuffer() {
        
        }

        GLuint ES3DesktopWin32IndexedVertexBuffer::getVAO() const {
            return _vao;
        }
        
        void *ES3DesktopWin32IndexedVertexBuffer::lockVertices() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            auto ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, _vlength, GL_MAP_WRITE_BIT);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return ptr;
        }

        void *ES3DesktopWin32IndexedVertexBuffer::lockIndices() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            auto ptr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, _ilength, GL_MAP_WRITE_BIT);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return ptr;
        }

        void ES3DesktopWin32IndexedVertexBuffer::unlockVertices() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void ES3DesktopWin32IndexedVertexBuffer::unlockIndices() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void ES3DesktopWin32IndexedVertexBuffer::release() {
            glDeleteBuffers(1, &_vbo);
            glDeleteBuffers(1, &_ibo);
            glDeleteVertexArrays(1, &_vao);
            delete this;
        }

        unsigned ES3DesktopWin32IndexedVertexBuffer::getVertexDataLength() const {
            return _vlength;
        }

        unsigned ES3DesktopWin32IndexedVertexBuffer::getIndexDataLength() const {
            return _ilength;
        }

        //---

        ES3DesktopWin32InstanceData::ES3DesktopWin32InstanceData(platform::InstanceDataType type, unsigned instanceCount) {
            unsigned index = unsigned(type);
            
            _length = instanceCount * __instanceParams[unsigned(type)].size;
            _type = type;
            _vbo = 0;
            
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, _length, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        ES3DesktopWin32InstanceData::~ES3DesktopWin32InstanceData() {

        }

        GLuint ES3DesktopWin32InstanceData::getVBO() const {
            return _vbo;
        }

        void *ES3DesktopWin32InstanceData::lock() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            auto ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, _length, GL_MAP_WRITE_BIT);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return ptr;
        }

        void ES3DesktopWin32InstanceData::unlock() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void ES3DesktopWin32InstanceData::update(const void *data, unsigned instanceCount) {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            auto ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, _length, GL_MAP_WRITE_BIT);
            memcpy(ptr, data, instanceCount * __instanceParams[unsigned(_type)].size); 
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void ES3DesktopWin32InstanceData::release() {
            glDeleteBuffers(1, &_vbo);
            delete this;
        }

        platform::InstanceDataType ES3DesktopWin32InstanceData::getType() const {
            return _type;
        }

        //---

        ES3DesktopWin32RasterizerParams::ES3DesktopWin32RasterizerParams(platform::CullMode cull) {
            _cullMode = cull;
        }

        ES3DesktopWin32RasterizerParams::~ES3DesktopWin32RasterizerParams() {
        
        }

        void ES3DesktopWin32RasterizerParams::release() {
            delete this;
        }

        void ES3DesktopWin32RasterizerParams::set() {
            if(_cullMode == platform::CullMode::NONE) {
                glDisable(GL_CULL_FACE);
            }
            else {
                glEnable(GL_CULL_FACE);
                glFrontFace(_cullMode == platform::CullMode::FRONT ? GL_CW : GL_CCW);                
            }
        }

        //---

        ES3DesktopWin32BlenderParams::ES3DesktopWin32BlenderParams(const platform::BlendMode blendMode) {
            _blendMode = blendMode;
        }

        ES3DesktopWin32BlenderParams::~ES3DesktopWin32BlenderParams() {
        
        }

        void ES3DesktopWin32BlenderParams::release() {
            delete this;
        }

        void ES3DesktopWin32BlenderParams::set() {
            if(_blendMode == platform::BlendMode::NOBLEND) {
                glDisable(GL_BLEND);
            }
            else {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, _blendMode == platform::BlendMode::ALPHA_ADD ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);
            }            
        }

        //---

        ES3DesktopWin32DepthParams::ES3DesktopWin32DepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) {
            _depthEnabled = depthEnabled;
            _depthWriteEnabled = depthWriteEnabled;
            _cmpFunc = __nativeCmpFunc[unsigned(compareFunc)];
        }

        ES3DesktopWin32DepthParams::~ES3DesktopWin32DepthParams() {

        }

        void ES3DesktopWin32DepthParams::release() {
            delete this;
        }

        void ES3DesktopWin32DepthParams::set() {
            _depthEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
            _depthWriteEnabled ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
            glDepthFunc(_cmpFunc);
        }

        //---

        ES3DesktopWin32Sampler::ES3DesktopWin32Sampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode, float minLod, float bias) {
            _self = 0;
            glGenSamplers(1, &_self);

            glSamplerParameteri(_self, GL_TEXTURE_MIN_LOD, (GLint)(minLod));
            glSamplerParameteri(_self, GL_TEXTURE_MIN_FILTER, filter == platform::TextureFilter::POINT ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
            glSamplerParameteri(_self, GL_TEXTURE_MAG_FILTER, filter == platform::TextureFilter::POINT ? GL_NEAREST : GL_LINEAR);
            glSamplerParameteri(_self, GL_TEXTURE_WRAP_S, addrMode == platform::TextureAddressMode::CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glSamplerParameteri(_self, GL_TEXTURE_WRAP_T, addrMode == platform::TextureAddressMode::CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        }

        ES3DesktopWin32Sampler::~ES3DesktopWin32Sampler() {

        }

        void ES3DesktopWin32Sampler::release() {
            glDeleteSamplers(1, &_self);
            delete this;
        }

        void ES3DesktopWin32Sampler::set(platform::TextureSlot slot) {
            glBindSampler(unsigned(slot), _self);
        }

        //---

        ES3DesktopWin32Shader::ES3DesktopWin32Shader(const byteinput &binary, const diag::LogInterface &log) {
            _program = 0;
            _vShader = 0;
            _fShader = 0;
            _program = glCreateProgram();
            _vShader = glCreateShader(GL_VERTEX_SHADER);
            _fShader = glCreateShader(GL_FRAGMENT_SHADER);

            GLint status = 0;
            const char *vshader = binary.getPtr();
            const char *fshader = vshader;

            while(*(unsigned *)(++fshader) != *(unsigned *)"-@@-") ;
            
            GLint vlen = unsigned(fshader - vshader);
            GLint flen = binary.getSize() - vlen - sizeof(unsigned);

            fshader += sizeof(unsigned);

            glShaderSource(_vShader, 1, &vshader, &vlen);
            glCompileShader(_vShader);
            glGetShaderiv(_vShader, GL_COMPILE_STATUS, &status);

            if(status != GL_TRUE) {
                GLint  length = 0;
                glGetShaderInfoLog(_vShader, __BUFFER_MAX, &length, __buffer);
                log.msgError("ES3DesktopWin32Shader::ES3DesktopWin32Shader / vshader / %s", __buffer);
            }

            glShaderSource(_fShader, 1, &fshader, &flen);
            glCompileShader(_fShader);
            glGetShaderiv(_fShader, GL_COMPILE_STATUS, &status);

            if(status != GL_TRUE) {
                GLint  length = 0;
                glGetShaderInfoLog(_fShader, __BUFFER_MAX, &length, __buffer);
                log.msgError("ES3DesktopWin32Shader::ES3DesktopWin32Shader / fshader / %s", __buffer);
            }

            glAttachShader(_program, _vShader);
            glAttachShader(_program, _fShader);

            for(unsigned i = 0; i < __VERTEX_COMPONENT_MAX; i++) {
                glBindAttribLocation(_program, i, __vertexComp[i]);
            }

            for (unsigned i = 0; i < __INSTANCE_COMPONENT_MAX; i++) {
                glBindAttribLocation(_program, __INSTANCE_COMPONENT_START + i, __instanceComp[i]);
            }
            
            glLinkProgram(_program);
            glGetProgramiv(_program, GL_LINK_STATUS, &status);

            if(status != GL_TRUE) {
                GLint  length = 0;
                glGetProgramInfoLog(_program, __BUFFER_MAX, &length, __buffer);
                log.msgError("ES3DesktopWin32Shader::ES3DesktopWin32Shader / program / %s", __buffer);
            }

            for(unsigned i = 0; i < __UBO_NAMES_MAX; i++) {
                GLuint uboPos = glGetUniformBlockIndex(_program, __uboNames[i]);
                
                if(uboPos != -1) {
                    glUniformBlockBinding(_program, uboPos, i);
                }
            }

            glUseProgram(_program);

            for(unsigned i = 0; i < platform::TEXTURE_UNITS_MAX; i++) {
                _textureLocations[i] = glGetUniformLocation(_program, __textureSamplerNames[i]);

                if(_textureLocations[i] != -1) {
                    glUniform1i(_textureLocations[i], i);
                }
                else {
                    _textureLocations[i] = 0;
                }
            }
        }

        ES3DesktopWin32Shader::~ES3DesktopWin32Shader() {

        }

        void ES3DesktopWin32Shader::set() {
            glUseProgram(_program);
        }

        void ES3DesktopWin32Shader::release() {
            glDeleteShader(_vShader);
            glDeleteShader(_fShader);
            glDeleteProgram(_program);
            delete this;
        }

        //---

        ES3DesktopWin32ShaderConstantBuffer::ES3DesktopWin32ShaderConstantBuffer(unsigned index, unsigned length) {
            _index = index;
            _length = length;
            glGenBuffers(1, &_ubo);
        }

        ES3DesktopWin32ShaderConstantBuffer::~ES3DesktopWin32ShaderConstantBuffer() {

        }

        void ES3DesktopWin32ShaderConstantBuffer::set() {
            glBindBufferBase(GL_UNIFORM_BUFFER, _index, _ubo);
        }

        void ES3DesktopWin32ShaderConstantBuffer::update(const void *data, unsigned byteWidth) {
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
            glBufferData(GL_UNIFORM_BUFFER, _length, data, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void ES3DesktopWin32ShaderConstantBuffer::release() {
            glDeleteBuffers(1, &_ubo);
            delete this;
        }

        //---
        
        ES3DesktopWin32Texture2D::ES3DesktopWin32Texture2D() {
            _texture = 0;
            _width = 0;
            _height = 0;
            _mipCount = 0;
            _format = platform::TextureFormat::UNKNOWN;
        }

        ES3DesktopWin32Texture2D::ES3DesktopWin32Texture2D(platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            _format = fmt;
            _width = originWidth;
            _height = originHeight;
            _mipCount = mipCount;

            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexStorage2D(GL_TEXTURE_2D, mipCount, __nativeTextureInternalFormat[unsigned(fmt)], originWidth, originHeight);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        ES3DesktopWin32Texture2D::ES3DesktopWin32Texture2D(unsigned char *const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount, platform::TextureFormat fmt) {
            _format = fmt;
            _width = originWidth;
            _height = originHeight;
            _mipCount = mipCount;

            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexStorage2D(GL_TEXTURE_2D, mipCount, __nativeTextureInternalFormat[unsigned(_format)], originWidth, originHeight);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            for(unsigned i = 0; i < mipCount; i++) {
                unsigned curWidth = originWidth >> i;
                unsigned curHeight = originHeight >> i;

                glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, curWidth, curHeight, __nativeTextureFormat[unsigned(_format)], GL_UNSIGNED_BYTE, imgMipsBinaryData[i]);
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        ES3DesktopWin32Texture2D::~ES3DesktopWin32Texture2D() {
        
        }

        unsigned ES3DesktopWin32Texture2D::getWidth() const {
            return _width;
        }

        unsigned ES3DesktopWin32Texture2D::getHeight() const {
            return _height;
        }

        unsigned ES3DesktopWin32Texture2D::getMipCount() const {
            return _mipCount;
        }

        void ES3DesktopWin32Texture2D::update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) {
            glBindTexture(GL_TEXTURE_2D, _texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, mip, x, y, w, h, __nativeTextureFormat[unsigned(_format)], GL_UNSIGNED_BYTE, src);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void ES3DesktopWin32Texture2D::set(platform::TextureSlot slot) {            
            glActiveTexture(GL_TEXTURE0 + unsigned(slot)); //
            glBindTexture(GL_TEXTURE_2D, _texture);            
        }

        void ES3DesktopWin32Texture2D::release() {
            glDeleteTextures(1, &_texture);
            delete this;
        }

        //---

        ES3DesktopWin32RenderTarget::ES3DesktopWin32RenderTarget() {
            _fbo = 0;
            _colorTargetCount = 0;
        }

        ES3DesktopWin32RenderTarget::ES3DesktopWin32RenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) {
            _fbo = 0;
            _colorTargetCount = colorTargetCount;

            _depthTexture._format = platform::TextureFormat::UNKNOWN;
            _depthTexture._width = originWidth;
            _depthTexture._height = originHeight;
            _depthTexture._mipCount = 1;

            glGenFramebuffers(1, &_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

            glGenTextures(1, &_depthTexture._texture);
            glBindTexture(GL_TEXTURE_2D, _depthTexture._texture);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, originWidth, originHeight);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture._texture, 0);

            for(unsigned i = 0; i < _colorTargetCount; i++) {
                _renderTexture[i]._format = platform::TextureFormat::RGBA8;
                _renderTexture[i]._width = originWidth;
                _renderTexture[i]._height = originHeight;
                _renderTexture[i]._mipCount = 1;

                glGenTextures(1, &_renderTexture[i]._texture);
                glBindTexture(GL_TEXTURE_2D, _renderTexture[i]._texture);
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, originWidth, originHeight);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _renderTexture[i]._texture, 0);
            }

            //GLenum status;
            //status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        ES3DesktopWin32RenderTarget::~ES3DesktopWin32RenderTarget() {
            
        }

        platform::Texture2DInterface *ES3DesktopWin32RenderTarget::getDepthBuffer() {
            return &_depthTexture;
        }

        platform::Texture2DInterface *ES3DesktopWin32RenderTarget::getRenderBuffer(unsigned index) {
            return &_renderTexture[index];
        }
        
        unsigned ES3DesktopWin32RenderTarget::getRenderBufferCount() const {
            return _colorTargetCount;
        }
        
        void ES3DesktopWin32RenderTarget::release() {
            for(unsigned i = 0; i < platform::RENDERTARGETS_MAX; i++) {
                if(_renderTexture[i]._texture) {
                    glDeleteTextures(1, &_renderTexture[i]._texture);
                }
            }

            glDeleteTextures(1, &_depthTexture._texture);
            glDeleteFramebuffers(1, &_fbo);
            delete this;
        }

        void ES3DesktopWin32RenderTarget::set() {
            glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
            glViewport(0, 0, _renderTexture->_width, _renderTexture->_height);
        }

        //---

        ES3DesktopWin32Platform::ES3DesktopWin32Platform(const diag::LogInterface &log) : _log(log) {
            _hwnd = 0;
            _hdc = 0;
            _eglDisplay = nullptr;
            _eglConfig = nullptr;
            _eglSurface = nullptr;
            _eglContext = nullptr;
            _nativeWidth = 0.0f;
            _nativeHeight = 0.0f;
            _curRenderTarget = nullptr;
        }

        bool ES3DesktopWin32Platform::init(const platform::InitParams &initParams) {
            EGLint lastError = EGL_SUCCESS;
            DesktopWin32InitParams &params = (DesktopWin32InitParams &)initParams;
            
            _hwnd = params.hWindow;
            _hdc = GetDC(_hwnd);
            _eglDisplay = eglGetDisplay(_hdc);

            _nativeWidth = params.scrWidth;
            _nativeHeight = params.scrHeight;
            
            if(_eglDisplay != EGL_NO_DISPLAY) {
                EGLint eglMajorVersion;
                EGLint eglMinorVersion;
                
                if(eglInitialize(_eglDisplay, &eglMajorVersion, &eglMinorVersion)) {
                    EGLint configsReturned;
                    EGLint configurationAttributes[] = {
                        EGL_DEPTH_SIZE, 24,
                        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL_NONE,
                    };

                    if(eglChooseConfig(_eglDisplay, configurationAttributes, &_eglConfig, 1, &configsReturned) && configsReturned == 1) {
                        _eglSurface = eglCreateWindowSurface(_eglDisplay, _eglConfig, _hwnd, NULL);
                        lastError = eglGetError();
                        
                        if(_eglSurface != EGL_NO_SURFACE && lastError == EGL_SUCCESS) {
                            eglBindAPI(EGL_OPENGL_ES_API);
                            lastError = eglGetError();

                            if(lastError == EGL_SUCCESS) {
                                EGLint contextAttributes[] = {
                                    EGL_CONTEXT_CLIENT_VERSION, 3,
                                    //0x30FB, 1,
                                    EGL_NONE,
                                };

                                _eglContext = eglCreateContext(_eglDisplay, _eglConfig, NULL, contextAttributes);
                                lastError = eglGetError();
                                
                                if(lastError == EGL_SUCCESS) {
                                    eglMakeCurrent(_eglDisplay, _eglSurface, _eglSurface, _eglContext);
                                    lastError = eglGetError();

                                    if(lastError == EGL_SUCCESS) {
                                        eglSwapInterval(_eglDisplay, params.syncInterval);

                                        glEnable(GL_CULL_FACE);
                                        glFrontFace(GL_CW);

                                        glDisable(GL_BLEND);
                                        glEnable(GL_DEPTH_TEST);

                                        for(unsigned i = 0; i < platform::TEXTURE_UNITS_MAX; i++) {
                                            _lastTextureWidth[i] = 0.0f;
                                            _lastTextureHeight[i] = 0.0f;
                                        }

                                        _defaultRenderTarget._colorTargetCount = 1;
                                        _defaultRenderTarget._depthTexture._width = unsigned(_nativeWidth);
                                        _defaultRenderTarget._depthTexture._height = unsigned(_nativeHeight);
                                        _defaultRenderTarget._depthTexture._mipCount = 1;
                                        _defaultRenderTarget._renderTexture[0]._width = unsigned(_nativeWidth);
                                        _defaultRenderTarget._renderTexture[0]._height = unsigned(_nativeHeight);
                                        _defaultRenderTarget._renderTexture[0]._mipCount = 1;

                                        _curRenderTarget = &_defaultRenderTarget;
                                        return true;
                                    }
                                }
                            }
                        }

                        _log.msgError("ES3DesktopWin32Platform::init / can't initialize OpelGL ES 3: lastError = %x", lastError);
                        eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                        eglTerminate(_eglDisplay);
                        ReleaseDC(_hwnd, _hdc);
                        return false;
                    }
                }
            }

            _log.msgError("ES3DesktopWin32Platform::init / can't initialize OpelGL ES 3: display/config initialization error", lastError);
            ReleaseDC(_hwnd, _hdc);
            return false;
        }

        void ES3DesktopWin32Platform::destroy() {
            eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglTerminate(_eglDisplay);
            ReleaseDC(_hwnd, _hdc);

            _eglDisplay = nullptr;
            _eglConfig = nullptr;
            _eglSurface = nullptr;
            _eglContext = nullptr;
        }

        float ES3DesktopWin32Platform::getScreenWidth() const {
            return _nativeWidth;
        }

        float ES3DesktopWin32Platform::getScreenHeight() const {
            return _nativeHeight;
        }

        float ES3DesktopWin32Platform::getCurrentRTWidth() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getWidth());
        }

        float ES3DesktopWin32Platform::getCurrentRTHeight() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getHeight());
        }

        float ES3DesktopWin32Platform::getTextureWidth(platform::TextureSlot slot) const {
            return _lastTextureWidth[unsigned(slot)];
        }

        float ES3DesktopWin32Platform::getTextureHeight(platform::TextureSlot slot) const {
            return _lastTextureHeight[unsigned(slot)];
        }

        unsigned ES3DesktopWin32Platform::getMemoryUsing() const {
            return 0;
        }

        unsigned ES3DesktopWin32Platform::getMemoryLimit() const {
            return 0;
        }

        unsigned long long ES3DesktopWin32Platform::getTimeMs() const {
            unsigned __int64 ttime;
            GetSystemTimeAsFileTime((FILETIME *)&ttime);
            return ttime / 10000;
        }

        void ES3DesktopWin32Platform::updateOrientation() {
        
        }

        void ES3DesktopWin32Platform::resize(float width, float height) {
            glViewport(0, 0, GLsizei(width), GLsizei(height));
        }

        const math::m3x3 &ES3DesktopWin32Platform::getInputTransform() const {
            static math::m3x3 _idmat;
            return _idmat;
        }

        void ES3DesktopWin32Platform::fsFormFilesList(const char *path, std::string &out) {
            struct fn {
                static void formList(const char *path, std::string &out) {
                    char     searchStr[260];
                    unsigned soff = 0;

                    for(; path[soff] != 0; soff++) searchStr[soff] = path[soff];
                    searchStr[soff++] = '\\';
                    searchStr[soff++] = '*';
                    searchStr[soff++] = '.';
                    searchStr[soff++] = '*';
                    searchStr[soff++] = 0;

                    WIN32_FIND_DATAA data;
                    HANDLE findHandle = FindFirstFileA(searchStr, &data);

                    if(findHandle != HANDLE(-1)) {
                        do {
                            if(data.cFileName[0] != '.') {
                                if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                                    unsigned int toff = 0;
                                    char         tpath[260];

                                    for(; path[toff] != 0; toff++) tpath[toff] = path[toff];
                                    tpath[toff++] = '\\';
                                    for(unsigned c = 0; data.cFileName[c] != 0; c++, toff++) tpath[toff] = data.cFileName[c];
                                    tpath[toff] = 0;

                                    formList(tpath, out);
                                }
                                else {
                                    out += path;
                                    out += "\\";
                                    out += data.cFileName;
                                    out += "\n";
                                }
                            }
                        }
                        while(FindNextFileA(findHandle, &data));
                    }
                }
            };

            fn::formList(path, out);
        }

        bool ES3DesktopWin32Platform::fsLoadFile(const char *path, void **oBinaryDataPtr, unsigned int *oSize) {
            FILE *fp = nullptr;
            fopen_s(&fp, path, "rb");

            if(fp) {
                fseek(fp, 0, SEEK_END);
                *oSize = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                *oBinaryDataPtr = new char[*oSize];
                fread(*oBinaryDataPtr, 1, *oSize, fp);

                fclose(fp);
                return true;
            }
            return false;
        }

        bool ES3DesktopWin32Platform::fsSaveFile(const char *path, void *iBinaryDataPtr, unsigned iSize) {
            FILE *fp = nullptr;
            fopen_s(&fp, path, "wb");

            if(fp) {
                fwrite(iBinaryDataPtr, 1, iSize, fp);
                fclose(fp);
                return true;
            }
            return false;
        }

        void ES3DesktopWin32Platform::sndSetGlobalVolume(float volume) {
        
        }

        platform::SoundEmitterInterface *ES3DesktopWin32Platform::sndCreateEmitter(unsigned sampleRate, unsigned channels) {
            return nullptr;
        }

        platform::VertexBufferInterface *ES3DesktopWin32Platform::rdCreateVertexBuffer(platform::VertexType vtype, unsigned vcount, bool isDynamic, const void *data) {
            auto ptr = new ES3DesktopWin32VertexBuffer (vtype, vcount, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if(data) {
                memcpy(ptr->lock(), data, ptr->getLength());
                ptr->unlock();
            }

            return ptr; 
        }

        platform::IndexedVertexBufferInterface *ES3DesktopWin32Platform::rdCreateIndexedVertexBuffer(platform::VertexType vtype, unsigned vcount, unsigned ushortIndexCount, bool isDynamic, const void *vdata, const void *idata) {
            auto ptr = new ES3DesktopWin32IndexedVertexBuffer(vtype, vcount, ushortIndexCount, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if(vdata) {
                memcpy(ptr->lockVertices(), vdata, ptr->getVertexDataLength());
                ptr->unlockVertices();
            }
            if(idata) {
                memcpy(ptr->lockIndices(), idata, ptr->getIndexDataLength());
                ptr->unlockIndices();
            }

            return ptr;
        }

        platform::InstanceDataInterface *ES3DesktopWin32Platform::rdCreateInstanceData(platform::InstanceDataType type, unsigned instanceCount) {
            return new ES3DesktopWin32InstanceData (type, instanceCount);
        }

        platform::ShaderInterface *ES3DesktopWin32Platform::rdCreateShader(const byteinput &binary) {
            return new ES3DesktopWin32Shader (binary, _log);
        }

        platform::RasterizerParamsInterface *ES3DesktopWin32Platform::rdCreateRasterizerParams(platform::CullMode cull) {
            return new ES3DesktopWin32RasterizerParams (cull);
        }

        platform::BlenderParamsInterface *ES3DesktopWin32Platform::rdCreateBlenderParams(const platform::BlendMode blendMode) {
            return new ES3DesktopWin32BlenderParams (blendMode);
        }

        platform::DepthParamsInterface *ES3DesktopWin32Platform::rdCreateDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) {
            return new ES3DesktopWin32DepthParams (depthEnabled, compareFunc, depthWriteEnabled);
        }

        platform::SamplerInterface *ES3DesktopWin32Platform::rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode, float minLod, float bias) {
            return new ES3DesktopWin32Sampler (filter, addrMode, minLod, bias);
        }

        platform::ShaderConstantBufferInterface *ES3DesktopWin32Platform::rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) {
            return new ES3DesktopWin32ShaderConstantBuffer (unsigned(appoint), byteWidth);
        }

        platform::Texture2DInterface *ES3DesktopWin32Platform::rdCreateTexture2D(unsigned char *const *imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount, platform::TextureFormat fmt) {
            return new ES3DesktopWin32Texture2D (imgMipsBinaryData, originWidth, originHeight, mipCount, fmt);
        }

        platform::Texture2DInterface *ES3DesktopWin32Platform::rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            return new ES3DesktopWin32Texture2D (format, originWidth, originHeight, mipCount);
        }

        platform::TextureCubeInterface *ES3DesktopWin32Platform::rdCreateTextureCube(unsigned char **imgMipsBinaryData[6], unsigned originSize, unsigned mipCount, platform::TextureFormat fmt) {
            return nullptr;
        }

        platform::RenderTargetInterface *ES3DesktopWin32Platform::rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) {
            return new ES3DesktopWin32RenderTarget (colorTargetCount, originWidth, originHeight);
        }

        platform::RenderTargetInterface *ES3DesktopWin32Platform::rdGetDefaultRenderTarget() {
            return &_defaultRenderTarget;
        }

        void ES3DesktopWin32Platform::rdClearCurrentDepthBuffer(float depth) {
            glClearDepthf(depth);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        void ES3DesktopWin32Platform::rdClearCurrentColorBuffer(const fg::color &c) {
            glClearColor(c.r, c.g, c.b, c.a);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        void ES3DesktopWin32Platform::rdSetRenderTarget(const platform::RenderTargetInterface *rt) {
            ES3DesktopWin32RenderTarget *platfromObject = (ES3DesktopWin32RenderTarget *)rt;
            _curRenderTarget = platfromObject;
            platfromObject->set();
        }

        void ES3DesktopWin32Platform::rdSetShader(const platform::ShaderInterface *vshader) {
            ES3DesktopWin32Shader *platfromObject = (ES3DesktopWin32Shader *)vshader;
            platfromObject->set();
        }

        void ES3DesktopWin32Platform::rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) {
            ES3DesktopWin32RasterizerParams *platfromObject = (ES3DesktopWin32RasterizerParams *)params;
            platfromObject->set();
        }

        void ES3DesktopWin32Platform::rdSetBlenderParams(const platform::BlenderParamsInterface *params) {
            ES3DesktopWin32BlenderParams *platformObject = (ES3DesktopWin32BlenderParams *)params;
            platformObject->set();
        }

        void ES3DesktopWin32Platform::rdSetDepthParams(const platform::DepthParamsInterface *params) {
            ES3DesktopWin32DepthParams *platfromObject = (ES3DesktopWin32DepthParams *)params;
            platfromObject->set();
        }

        void ES3DesktopWin32Platform::rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) {
            ES3DesktopWin32Sampler *platfromObject = (ES3DesktopWin32Sampler *)sampler;
            platfromObject->set(slot);
        }

        void ES3DesktopWin32Platform::rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) {
            ES3DesktopWin32ShaderConstantBuffer *platfromObject = (ES3DesktopWin32ShaderConstantBuffer *)cbuffer;
            platfromObject->set();
        }

        void ES3DesktopWin32Platform::rdSetTexture2D(platform::TextureSlot slot, const platform::Texture2DInterface *texture) {
            ES3DesktopWin32Texture2D *platfromObject = (ES3DesktopWin32Texture2D *)texture;
            
            if (platfromObject) {
                platfromObject->set(slot);

                _lastTextureWidth[unsigned(slot)] = float(platfromObject->getWidth());
                _lastTextureHeight[unsigned(slot)] = float(platfromObject->getHeight());
            }
        }

        void ES3DesktopWin32Platform::rdSetTextureCube(platform::TextureSlot slot, const platform::TextureCubeInterface *texture) {

        }
        
        void ES3DesktopWin32Platform::rdSetScissorRect(const math::p2d &topLeft, const math::p2d &bottomRight) {
            glScissor(int(topLeft.x), int(topLeft.y), int(bottomRight.x - topLeft.x), int(bottomRight.y - topLeft.y));
        }
        
        void ES3DesktopWin32Platform::rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned vertexCount, unsigned instanceCount) {
            ES3DesktopWin32VertexBuffer *platfromObject = (ES3DesktopWin32VertexBuffer *)vbuffer;
            ES3DesktopWin32InstanceData *platformInstanceData = (ES3DesktopWin32InstanceData *)instanceData;
            InstanceParams &curParams = __instanceParams[unsigned(platformInstanceData->getType())];

            glBindVertexArray(platfromObject->getVAO());
            glBindBuffer(GL_ARRAY_BUFFER, platformInstanceData->getVBO());

            unsigned offset = 0;
            unsigned startIndex = __INSTANCE_COMPONENT_START;

            for(unsigned i = startIndex; i < startIndex + curParams.layoutCount; i++) {
                glVertexAttribPointer(i, curParams.floatCounts[i - startIndex], GL_FLOAT, GL_FALSE, 0, (const GLvoid *)offset);
                glVertexAttribDivisor(i, 1);
                glEnableVertexAttribArray(i);
                offset += curParams.floatCounts[i - startIndex] * sizeof(float);
            }

            glDrawArraysInstanced(__nativeTopology[unsigned(topology)], 0, vertexCount, instanceCount); //
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void ES3DesktopWin32Platform::rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, const platform::InstanceDataInterface *instanceData, platform::PrimitiveTopology topology, unsigned indexCount, unsigned instanceCount) {
            ES3DesktopWin32IndexedVertexBuffer *platfromObject = (ES3DesktopWin32IndexedVertexBuffer *)ivbuffer;
            ES3DesktopWin32InstanceData *platformInstanceData = (ES3DesktopWin32InstanceData *)instanceData;
            InstanceParams &curParams = __instanceParams[unsigned(platformInstanceData->getType())];
            
            glBindVertexArray(platfromObject->getVAO());
            glBindBuffer(GL_ARRAY_BUFFER, platformInstanceData->getVBO());

            unsigned offset = 0;
            unsigned startIndex = __INSTANCE_COMPONENT_START;
            
            for (unsigned i = startIndex; i < startIndex + curParams.layoutCount; i++) {
                glVertexAttribPointer(i, curParams.floatCounts[i - startIndex], GL_FLOAT, GL_FALSE, 0, (const GLvoid *)offset);
                glVertexAttribDivisor(i, 1);
                glEnableVertexAttribArray(i);
                offset += curParams.floatCounts[i - startIndex] * sizeof(float);
            }

            glDrawElementsInstanced(__nativeTopology[unsigned(topology)], indexCount, GL_UNSIGNED_SHORT, nullptr, instanceCount); //
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        
        void ES3DesktopWin32Platform::rdPresent() {            
            if(!eglSwapBuffers(_eglDisplay, _eglSurface)) {
                _log.msgError("ES3DesktopWin32Platform::rdPresent / opengl %x error\n", eglGetError());
            }
        }

        bool ES3DesktopWin32Platform::isInited() const {
            return _eglContext != nullptr;
        }
    }
}




