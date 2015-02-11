
#include "pch.h"

namespace fg {
    namespace opengl {
        const unsigned __VERTEX_PARAMS_MAX     = 5;
        const unsigned __UBO_NAMES_MAX         = 5;
        const unsigned __VERTEX_COMPONENT_MAX  = 7;
        const unsigned __BUFFER_MAX            = 4096;
        
        struct VertexParams {
            unsigned size;
            unsigned layoutCount;
            unsigned floatCounts[16];
        } 
        __vertexParams[__VERTEX_PARAMS_MAX] = {
            {3 * sizeof(float), 1, {3}},
            {5 * sizeof(float), 2, {3, 2}},
            {14 * sizeof(float), 5, {3, 2, 3, 3, 3}},
            {13 * sizeof(float), 4, {3, 2, 4, 4}},
            {22 * sizeof(float), 7, {3, 2, 3, 3, 3, 4, 4}},
        };

        const char *__uboNames[__UBO_NAMES_MAX] = {
            "FrameData",
            "DrawData",
            "MaterialData",
            "SkinData",
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
        GLenum __nativeTextureFormat[] = {GL_RGBA, GL_RED, GL_RED};
        GLenum __nativeTextureInternalFormat[] = {GL_RGBA8, GL_R8, GL_R8};
        GLenum __nativeTopology[] = {GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP};
        GLenum __nativeCmpFunc[] = {GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS};

        //---

        ES3DesktopVertexBuffer::ES3DesktopVertexBuffer(platform::VertexType type, unsigned vcount, GLenum usage) {
            unsigned index = unsigned(type);
            unsigned offset = 0;
            
            _vao = 0;
            _vbo = 0;
            _usage = usage;

            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);       
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            
            for(unsigned i = 0; i < __vertexParams[index].layoutCount; i++) {
                glVertexAttribPointer(i, __vertexParams[index].floatCounts[i], GL_FLOAT, GL_FALSE, __vertexParams[index].size, (const GLvoid *)offset);
                glEnableVertexAttribArray(i);

                offset += __vertexParams[index].floatCounts[i] * sizeof(float);
            }

            _length = offset * vcount;
            glBufferData(GL_ARRAY_BUFFER, _length, nullptr, _usage);
            glBindVertexArray(0);
        }

        ES3DesktopVertexBuffer::~ES3DesktopVertexBuffer() {

        }

        GLuint ES3DesktopVertexBuffer::getVAO() const {
            return _vao;
        }

        void *ES3DesktopVertexBuffer::lock() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            return glMapBufferRange(GL_ARRAY_BUFFER, 0, _length, GL_MAP_WRITE_BIT);
        }

        void ES3DesktopVertexBuffer::unlock() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void ES3DesktopVertexBuffer::update(void *data) {            
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, _length, data, _usage);
        }
       
        void ES3DesktopVertexBuffer::release() {
            glDeleteBuffers(1, &_vbo);
            glDeleteVertexArrays(1, &_vao);
            delete this;
        }

        //---

        ES3DesktopIndexedVertexBuffer::ES3DesktopIndexedVertexBuffer(platform::VertexType type, unsigned vcount, unsigned icount, GLenum usage) {
            unsigned index = unsigned(type);
            unsigned offset = 0;

            _vao = 0;
            _ibo = 0;
            _vbo = 0;
            _usage = usage;

            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);
            
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glGenBuffers(1, &_ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

            for(unsigned i = 0; i < __vertexParams[index].layoutCount; i++) {
                glVertexAttribPointer(i, __vertexParams[index].floatCounts[i], GL_FLOAT, GL_FALSE, __vertexParams[index].size, (const GLvoid *)offset);
                glEnableVertexAttribArray(i);

                offset += __vertexParams[index].floatCounts[i] * sizeof(float);
            }

            _vlength = offset * vcount;
            _ilength = icount * sizeof(unsigned short);

            glBufferData(GL_ARRAY_BUFFER, _vlength, nullptr, _usage);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _ilength, nullptr, _usage);
            glBindVertexArray(0);
        }

        ES3DesktopIndexedVertexBuffer::~ES3DesktopIndexedVertexBuffer() {
        
        }

        GLuint ES3DesktopIndexedVertexBuffer::getVAO() const {
            return _vao;
        }
        
        void *ES3DesktopIndexedVertexBuffer::lockVertices() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            return glMapBufferRange(GL_ARRAY_BUFFER, 0, _vlength, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }

        void *ES3DesktopIndexedVertexBuffer::lockIndices() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            return glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, _ilength, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        }

        void ES3DesktopIndexedVertexBuffer::unlockVertices() {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void ES3DesktopIndexedVertexBuffer::unlockIndices() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }

        void ES3DesktopIndexedVertexBuffer::updateVertices(void *data) {
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, _vlength, data, _usage);
        }

        void ES3DesktopIndexedVertexBuffer::updateIndices(void *data) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _ilength, data, _usage);
        }

        void ES3DesktopIndexedVertexBuffer::release() {
            glDeleteBuffers(1, &_vbo);
            glDeleteBuffers(1, &_ibo);
            glDeleteVertexArrays(1, &_vao);
            delete this;
        }

        //---

        ES3DesktopRasterizerParams::ES3DesktopRasterizerParams(platform::CullMode cull) {
            _cullMode = cull;
        }

        ES3DesktopRasterizerParams::~ES3DesktopRasterizerParams() {
        
        }

        void ES3DesktopRasterizerParams::release() {
            delete this;
        }

        void ES3DesktopRasterizerParams::set() {
            if(_cullMode == platform::CullMode::NONE) {
                glDisable(GL_CULL_FACE);
            }
            else {
                glEnable(GL_CULL_FACE);
                glFrontFace(_cullMode == platform::CullMode::FRONT ? GL_CW : GL_CCW);                
            }
        }

        //---

        ES3DesktopBlenderParams::ES3DesktopBlenderParams(const platform::BlendMode blendMode) {
            _blendMode = blendMode;
        }

        ES3DesktopBlenderParams::~ES3DesktopBlenderParams() {
        
        }

        void ES3DesktopBlenderParams::release() {
            delete this;
        }

        void ES3DesktopBlenderParams::set() {
            if(_blendMode == platform::BlendMode::NOBLEND) {
                glDisable(GL_BLEND);
            }
            else {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, _blendMode == platform::BlendMode::ALPHA_ADD ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);
            }            
        }

        //---

        ES3DesktopDepthParams::ES3DesktopDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) {
            _depthEnabled = depthEnabled;
            _depthWriteEnabled = depthWriteEnabled;
            _cmpFunc = __nativeCmpFunc[unsigned(compareFunc)];
        }

        ES3DesktopDepthParams::~ES3DesktopDepthParams() {

        }

        void ES3DesktopDepthParams::release() {
            delete this;
        }

        void ES3DesktopDepthParams::set() {
            _depthEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
            _depthWriteEnabled ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
            glDepthFunc(_cmpFunc);
        }

        //---

        ES3DesktopSampler::ES3DesktopSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode) {
            _self = 0;
            glGenSamplers(1, &_self);

            glSamplerParameteri(_self, GL_TEXTURE_MIN_FILTER, filter == platform::TextureFilter::POINT ? GL_NEAREST : GL_LINEAR);
            glSamplerParameteri(_self, GL_TEXTURE_MAG_FILTER, filter == platform::TextureFilter::POINT ? GL_NEAREST : GL_LINEAR);
            glSamplerParameteri(_self, GL_TEXTURE_WRAP_S, addrMode == platform::TextureAddressMode::CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glSamplerParameteri(_self, GL_TEXTURE_WRAP_T, addrMode == platform::TextureAddressMode::CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        }

        ES3DesktopSampler::~ES3DesktopSampler() {

        }

        void ES3DesktopSampler::release() {
            glDeleteSamplers(1, &_self);
            delete this;
        }

        void ES3DesktopSampler::set(platform::TextureSlot slot) {
            glBindSampler(unsigned(slot), _self);
        }

        //---

        ES3DesktopShader::ES3DesktopShader(const byteform &binary, const diag::LogInterface &log) {
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
                log.msgError("ES3DesktopShader::ES3DesktopShader / vshader / %s", __buffer);
            }

            glShaderSource(_fShader, 1, &fshader, &flen);
            glCompileShader(_fShader);
            glGetShaderiv(_fShader, GL_COMPILE_STATUS, &status);

            if(status != GL_TRUE) {
                GLint  length = 0;
                glGetShaderInfoLog(_fShader, __BUFFER_MAX, &length, __buffer);
                log.msgError("ES3DesktopShader::ES3DesktopShader / fshader / %s", __buffer);
            }

            glAttachShader(_program, _vShader);
            glAttachShader(_program, _fShader);

            for(unsigned i = 0; i < __VERTEX_COMPONENT_MAX; i++) {
                glBindAttribLocation(_program, i, __vertexComp[i]);
            }
            
            glLinkProgram(_program);
            glGetProgramiv(_program, GL_LINK_STATUS, &status);

            if(status != GL_TRUE) {
                GLint  length = 0;
                glGetProgramInfoLog(_program, __BUFFER_MAX, &length, __buffer);
                log.msgError("ES3DesktopShader::ES3DesktopShader / program / %s", __buffer);
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
            }
        }

        ES3DesktopShader::~ES3DesktopShader() {

        }

        void ES3DesktopShader::set() {
            glUseProgram(_program);
        }

        void ES3DesktopShader::release() {
            glDeleteShader(_vShader);
            glDeleteShader(_fShader);
            glDeleteProgram(_program);
            delete this;
        }

        //---

        ES3DesktopShaderConstantBuffer::ES3DesktopShaderConstantBuffer(unsigned index, unsigned length) {
            _index = index;
            _length = length;
            glGenBuffers(1, &_ubo);
        }

        ES3DesktopShaderConstantBuffer::~ES3DesktopShaderConstantBuffer() {

        }

        void ES3DesktopShaderConstantBuffer::set() {
            glBindBufferBase(GL_UNIFORM_BUFFER, _index, _ubo);
        }

        void ES3DesktopShaderConstantBuffer::update(const void *data) const {
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
            glBufferData(GL_UNIFORM_BUFFER, _length, data, GL_DYNAMIC_DRAW);
        }

        void ES3DesktopShaderConstantBuffer::release() {
            glDeleteBuffers(1, &_ubo);
            delete this;
        }

        //---
        
        ES3DesktopTexture2D::ES3DesktopTexture2D() {
            _texture = 0;
            _width = 0;
            _height = 0;
            _mipCount = 0;
            _format = platform::TextureFormat::UNKNOWN;
        }

        ES3DesktopTexture2D::ES3DesktopTexture2D(platform::TextureFormat fmt, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            _format = fmt;
            _width = originWidth;
            _height = originHeight;
            _mipCount = mipCount;

            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);            
            glTexStorage2D(GL_TEXTURE_2D, mipCount, __nativeTextureInternalFormat[unsigned(fmt)], originWidth, originHeight);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        ES3DesktopTexture2D::~ES3DesktopTexture2D() {
        
        }

        unsigned ES3DesktopTexture2D::getWidth() const {
            return _width;
        }

        unsigned ES3DesktopTexture2D::getHeight() const {
            return _height;
        }

        unsigned ES3DesktopTexture2D::getMipCount() const {
            return _mipCount;
        }

        void ES3DesktopTexture2D::update(unsigned mip, unsigned x, unsigned y, unsigned w, unsigned h, void *src) {
            glBindTexture(GL_TEXTURE_2D, _texture);
            glTexSubImage2D(GL_TEXTURE_2D, mip, x, y, w, h, __nativeTextureFormat[unsigned(_format)], GL_UNSIGNED_BYTE, src);
        }
        
        void *ES3DesktopTexture2D::getNativeHandle() const {
            return (void *)_texture;
        }

        void ES3DesktopTexture2D::set(platform::TextureSlot slot) {
            glActiveTexture(GL_TEXTURE0 + unsigned(slot));
            glBindTexture(GL_TEXTURE_2D, _texture);
            
        }

        void ES3DesktopTexture2D::release() {
            glDeleteTextures(1, &_texture);
            delete this;
        }

        //---

        ES3DesktopRenderTarget::ES3DesktopRenderTarget() {
            _fbo = 0;
            _colorTargetCount = 0;
        }

        ES3DesktopRenderTarget::ES3DesktopRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) {
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
        }

        ES3DesktopRenderTarget::~ES3DesktopRenderTarget() {
            
        }

        platform::Texture2DInterface *ES3DesktopRenderTarget::getDepthBuffer() {
            return &_depthTexture;
        }

        platform::Texture2DInterface *ES3DesktopRenderTarget::getRenderBuffer(unsigned index) {
            return &_renderTexture[index];
        }
        
        void ES3DesktopRenderTarget::release() {
            for(unsigned i = 0; i < platform::RENDERTARGETS_MAX; i++) {
                if(_renderTexture[i]._texture) {
                    glDeleteTextures(1, &_renderTexture[i]._texture);
                }
            }

            glDeleteTextures(1, &_depthTexture._texture);
            glDeleteFramebuffers(1, &_fbo);
            delete this;
        }

        void ES3DesktopRenderTarget::set() {
            glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
            glViewport(0, 0, _renderTexture->_width, _renderTexture->_height);
        }

        //---

        ES3DesktopPlatform::ES3DesktopPlatform(const diag::LogInterface &log) : _log(log) {
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

        bool ES3DesktopPlatform::init(const platform::InitParams &initParams) {
            EGLint lastError = EGL_SUCCESS;
            DesktopInitParams &params = (DesktopInitParams &)initParams;
            
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

                        _log.msgError("ES3DesktopPlatform::init / can't initialize OpelGL ES 3: lastError = %x", lastError);
                        eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                        eglTerminate(_eglDisplay);
                        ReleaseDC(_hwnd, _hdc);
                        return false;
                    }
                }
            }

            _log.msgError("ES3DesktopPlatform::init / can't initialize OpelGL ES 3: display/config initialization error", lastError);
            ReleaseDC(_hwnd, _hdc);
            return false;
        }

        void ES3DesktopPlatform::destroy() {
            eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglTerminate(_eglDisplay);
            ReleaseDC(_hwnd, _hdc);

            _eglDisplay = nullptr;
            _eglConfig = nullptr;
            _eglSurface = nullptr;
            _eglContext = nullptr;
        }

        float ES3DesktopPlatform::getScreenWidth() const {
            return _nativeWidth;
        }

        float ES3DesktopPlatform::getScreenHeight() const {
            return _nativeHeight;
        }

        float ES3DesktopPlatform::getCurrentRTWidth() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getWidth());
        }

        float ES3DesktopPlatform::getCurrentRTHeight() const {
            return float(_curRenderTarget->getRenderBuffer(0)->getHeight());
        }

        float ES3DesktopPlatform::getTextureWidth(platform::TextureSlot slot) const {
            return _lastTextureWidth[unsigned(slot)];
        }

        float ES3DesktopPlatform::getTextureHeight(platform::TextureSlot slot) const {
            return _lastTextureHeight[unsigned(slot)];
        }

        unsigned ES3DesktopPlatform::getMemoryUsing() const {
            return 0;
        }

        unsigned ES3DesktopPlatform::getMemoryLimit() const {
            return 0;
        }

        unsigned long long ES3DesktopPlatform::getTimeMs() const {
            unsigned __int64 ttime;
            GetSystemTimeAsFileTime((FILETIME *)&ttime);
            return ttime / 10000;
        }

        void ES3DesktopPlatform::updateOrientation() {
        
        }

        const math::m3x3 &ES3DesktopPlatform::getOrientationTransform2D() const {
            return _orientationTransform2D;
        }

        const math::m4x4 &ES3DesktopPlatform::getOrientationTransform3D() const {
            return _orientationTransform3D;
        }

        void ES3DesktopPlatform::fsFormFilesList(const char *path, std::string &out) {
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

        bool ES3DesktopPlatform::fsLoadFile(const char *path, void **oBinaryDataPtr, unsigned int *oSize) {
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

        void ES3DesktopPlatform::sndSetGlobalVolume(float volume) {
        
        }

        platform::SoundEmitterInterface *ES3DesktopPlatform::sndCreateEmitter(unsigned sampleRate, unsigned channels) {
            return nullptr;
        }

        platform::VertexBufferInterface *ES3DesktopPlatform::rdCreateVertexBuffer(platform::VertexType vtype, unsigned vcount, bool isDynamic, void *data) {
            auto ptr = new ES3DesktopVertexBuffer (vtype, vcount, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if(data) {
                ptr->update(data);
            }

            return ptr; 
        }

        platform::IndexedVertexBufferInterface *ES3DesktopPlatform::rdCreateIndexedVertexBuffer(platform::VertexType vtype, unsigned vcount, unsigned ushortIndexCount, bool isDynamic, void *vdata, void *idata) {
            auto ptr = new ES3DesktopIndexedVertexBuffer(vtype, vcount, ushortIndexCount, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if(vdata) {
                ptr->updateVertices(vdata);
            }
            if(idata) {
                ptr->updateIndices(idata);
            }

            return ptr;
        }

        platform::ShaderInterface *ES3DesktopPlatform::rdCreateShader(const byteform &binary) {
            return new ES3DesktopShader (binary, _log);
        }

        platform::RasterizerParamsInterface *ES3DesktopPlatform::rdCreateRasterizerParams(platform::CullMode cull) {
            return new ES3DesktopRasterizerParams (cull);
        }

        platform::BlenderParamsInterface *ES3DesktopPlatform::rdCreateBlenderParams(const platform::BlendMode blendMode) {
            return new ES3DesktopBlenderParams (blendMode);
        }

        platform::DepthParamsInterface *ES3DesktopPlatform::rdCreateDepthParams(bool depthEnabled, platform::DepthFunc compareFunc, bool depthWriteEnabled) {
            return new ES3DesktopDepthParams (depthEnabled, compareFunc, depthWriteEnabled);
        }

        platform::SamplerInterface *ES3DesktopPlatform::rdCreateSampler(platform::TextureFilter filter, platform::TextureAddressMode addrMode) {
            return new ES3DesktopSampler (filter, addrMode);
        }

        platform::ShaderConstantBufferInterface *ES3DesktopPlatform::rdCreateShaderConstantBuffer(platform::ShaderConstBufferUsing appoint, unsigned byteWidth) {
            return new ES3DesktopShaderConstantBuffer (unsigned(appoint), byteWidth);
        }

        platform::Texture2DInterface *ES3DesktopPlatform::rdCreateTexture2D(unsigned char **imgMipsBinaryData, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            return nullptr;
        }

        platform::Texture2DInterface *ES3DesktopPlatform::rdCreateTexture2D(platform::TextureFormat format, unsigned originWidth, unsigned originHeight, unsigned mipCount) {
            return new ES3DesktopTexture2D (format, originWidth, originHeight, mipCount);
        }

        platform::RenderTargetInterface *ES3DesktopPlatform::rdCreateRenderTarget(unsigned colorTargetCount, unsigned originWidth, unsigned originHeight) {
            return new ES3DesktopRenderTarget (colorTargetCount, originWidth, originHeight);
        }

        platform::RenderTargetInterface *ES3DesktopPlatform::rdGetDefaultRenderTarget() {
            return &_defaultRenderTarget;
        }

        void ES3DesktopPlatform::rdClearCurrentDepthBuffer(float depth) {
            glClearDepthf(depth);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        void ES3DesktopPlatform::rdClearCurrentColorBuffer(const platform::color &c) {
            glClearColor(c.r, c.g, c.b, c.a);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        void ES3DesktopPlatform::rdSetRenderTarget(const platform::RenderTargetInterface *rt) {
            ES3DesktopRenderTarget *platfromObject = (ES3DesktopRenderTarget *)rt;
            _curRenderTarget = platfromObject;
            platfromObject->set();
        }

        void ES3DesktopPlatform::rdSetShader(const platform::ShaderInterface *vshader) {
            ES3DesktopShader *platfromObject = (ES3DesktopShader *)vshader;
            platfromObject->set();
        }

        void ES3DesktopPlatform::rdSetRasterizerParams(const platform::RasterizerParamsInterface *params) {
            ES3DesktopRasterizerParams *platfromObject = (ES3DesktopRasterizerParams *)params;
            platfromObject->set();
        }

        void ES3DesktopPlatform::rdSetBlenderParams(const platform::BlenderParamsInterface *params) {
            ES3DesktopBlenderParams *platformObject = (ES3DesktopBlenderParams *)params;
            platformObject->set();
        }

        void ES3DesktopPlatform::rdSetDepthParams(const platform::DepthParamsInterface *params) {
            ES3DesktopDepthParams *platfromObject = (ES3DesktopDepthParams *)params;
            platfromObject->set();
        }

        void ES3DesktopPlatform::rdSetSampler(platform::TextureSlot slot, const platform::SamplerInterface *sampler) {
            ES3DesktopSampler *platfromObject = (ES3DesktopSampler *)sampler;
            platfromObject->set(slot);
        }

        void ES3DesktopPlatform::rdSetShaderConstBuffer(const platform::ShaderConstantBufferInterface *cbuffer) {
            ES3DesktopShaderConstantBuffer *platfromObject = (ES3DesktopShaderConstantBuffer *)cbuffer;
            platfromObject->set();
        }

        void ES3DesktopPlatform::rdSetTexture2D(platform::TextureSlot slot, const platform::Texture2DInterface *texture) {
            ES3DesktopTexture2D *platfromObject = (ES3DesktopTexture2D *)texture;
            platfromObject->set(slot);

            _lastTextureWidth[unsigned(slot)] = float(platfromObject->getWidth());
            _lastTextureHeight[unsigned(slot)] = float(platfromObject->getHeight());
        }
        
        void ES3DesktopPlatform::rdSetScissorRect(math::p2d &topLeft, math::p2d &bottomRight) {
            glScissor(int(topLeft.x), int(topLeft.y), int(bottomRight.x - topLeft.x), int(bottomRight.y - topLeft.y));
        }
        
        void ES3DesktopPlatform::rdDrawGeometry(const platform::VertexBufferInterface *vbuffer, platform::PrimitiveTopology topology, unsigned vertexCount) {
            ES3DesktopVertexBuffer *platfromObject = (ES3DesktopVertexBuffer *)vbuffer;
            glBindVertexArray(platfromObject->getVAO());
            glDrawArrays(__nativeTopology[unsigned(topology)], 0, vertexCount); //
            glBindVertexArray(0);
        }

        void ES3DesktopPlatform::rdDrawIndexedGeometry(const platform::IndexedVertexBufferInterface *ivbuffer, platform::PrimitiveTopology topology, unsigned indexCount) {
            ES3DesktopIndexedVertexBuffer *platfromObject = (ES3DesktopIndexedVertexBuffer *)ivbuffer;
            glBindVertexArray(platfromObject->getVAO());
            glDrawElements(__nativeTopology[unsigned(topology)], indexCount, GL_UNSIGNED_SHORT, nullptr); //
            glBindVertexArray(0);
        }
        
        void ES3DesktopPlatform::rdPresent() {            
            if(!eglSwapBuffers(_eglDisplay, _eglSurface)) {
                _log.msgError("ES3DesktopPlatform::rdPresent / opengl %x error\n", eglGetError());
            }
        }

        bool ES3DesktopPlatform::isInited() {
            return _eglContext != nullptr;
        }
    }
}




