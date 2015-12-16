
//TODO: margin, safe, optimization, full utf8

namespace fg {
    namespace resources {
        static const int FONT_ATLAS_SIZE = 512;
        static const int FONT_MIN_SIZE = 6;
        static const int FONT_MAX_SIZE = 128;
        static const int FONT_MIN_OFFSET = 4;

        FontResource::FontForm::Atlas::Atlas(const diag::LogInterface &log, platform::PlatformInterface &api) : hOffset(FONT_MIN_OFFSET), vOffset(FONT_MIN_OFFSET) {
            texture = api.rdCreateTexture2D(platform::TextureFormat::RED8, FONT_ATLAS_SIZE, FONT_ATLAS_SIZE, 1);

            if(texture == nullptr){
                log.msgError("FontResource::Atlas::Atlas can't create texture");
            }
        }

        FontResource::FontForm::Atlas::~Atlas() {
            if(texture) {
                texture->release();
            }
        }

        FontResource::FontForm::~FontForm() {
            for(auto index = chars.begin(); index != chars.end(); ++index){
                delete index->second;
            }
            
            Atlas *cur = atlasList;

            while(cur){
                Atlas *toDel = cur;
                cur = cur->nextPtr;
                delete toDel;
            }
        }
        
        FontResource::FontResource(const char *path, bool unloadable) : Resource(path, unloadable), _api(nullptr), _log(nullptr) {
            memset(&_self, 0, sizeof(tools::stbtt_fontinfo));
        }

        FontResource::~FontResource() {
            for(auto index = _fontForms.begin(); index != _fontForms.end(); ++index){
                delete index->second;
            }
        }

        void FontResource::loaded(const diag::LogInterface &log) {
            if(!stbtt_InitFont(&_self, (unsigned char *)_binaryData, 0)){
                log.msgError("ResFont::loaded %s bad format", _loadPath.data());
                _loadingState = ResourceLoadingState::INVALID;
            }
        }

        bool FontResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            _api = &api;
            _log = &log;
            return true; // keep binary in memory
        }

        void FontResource::unloaded() {
            for(auto index = _fontForms.begin(); index != _fontForms.end(); ++index){
                delete index->second;
            }

            _fontForms.clear();

            delete (char *)_binaryData;
            _binaryData = nullptr;
            _binarySize = 0;
        }

        unsigned FontResource::cache(const char *mbcharsz, unsigned fontSize, unsigned blur, unsigned maxCaching) {
            if(fontSize < FONT_MIN_SIZE || fontSize > FONT_MAX_SIZE) return 0;

            FontForm *curFontForm = nullptr;
            auto fontSzIndex = _fontForms.find((fontSize << 16) + blur);

            if(fontSzIndex != _fontForms.end()){
                curFontForm = fontSzIndex->second;
            }
            else{
                curFontForm = _cacheForm(fontSize, blur);
            }

            unsigned counter = 0;
            unsigned short ch = 0;
            unsigned chLen = 0;
            const char *charPtr = mbcharsz;

            for(; charPtr[0] != 0; charPtr += chLen){
                ch = string::utf8ToUTF16(charPtr, &chLen);

                auto charIndex = curFontForm->chars.find(ch);
                if(charIndex == curFontForm->chars.end()){
                    if (counter < maxCaching) {
                        _cacheChar(ch, curFontForm);
                        counter++;
                    }
                    else break;
                }
            }

            return unsigned(charPtr - mbcharsz);
        }

        void FontResource::getChar(const char *mbChar, unsigned fontSize, unsigned blur, FontCharInfo &out) const {
            getChar(string::utf8ToUTF16(mbChar), fontSize, blur, out);
        }

        void FontResource::getChar(unsigned short ch, unsigned fontSize, unsigned blur, FontCharInfo &out) const {
            FontForm  *curFontSize = nullptr;
            const CharData  *charData = nullptr;
            auto  fontSzIndex = _fontForms.find((fontSize << 16) + blur);

            if(fontSzIndex != _fontForms.end()) {
                curFontSize = fontSzIndex->second;
            }
            else {
                curFontSize = _cacheForm(fontSize, blur);
            }
            auto charIndex = curFontSize->chars.find(ch);

            if(charIndex == curFontSize->chars.end()) {
                charData = _cacheChar(ch, curFontSize);
            }
            else {
                charData = charIndex->second;
            }

            out.texture = charData->atlas->texture;
            out.tu = charData->tu;
            out.tv = charData->tv;
            out.txWidth = charData->txWidth;
            out.txHeight = charData->txHeight;
            out.width = charData->width;
            out.height = charData->height;
            out.advance = charData->advance;
            out.lsb = charData->lsb;
        }

        float FontResource::getTextWidth(const char *text, unsigned fontSize) const {
            float fontScale = stbtt_ScaleForPixelHeight(&_self, float(fontSize));
            float resultWidth = 0.0f;
            unsigned tchLen = 0;
            
            for(const char *charPtr = text; charPtr[0] != 0; charPtr += tchLen) {
                if (*charPtr == '\n') {
                    resultWidth = 0.0f;
                }
                else {
                    unsigned short ch = string::utf8ToUTF16(charPtr, &tchLen);

                    int glyph = stbtt_FindGlyphIndex(&_self, ch);
                    int iadvance, ilsb;

                    stbtt_GetGlyphHMetrics(&_self, glyph, &iadvance, &ilsb);

                    resultWidth += std::floor(fontScale * float(ilsb));
                    resultWidth += std::ceil(fontScale * float(iadvance) - fontScale * float(ilsb));
                }
            }

            return resultWidth;
        }
        
        float FontResource::getLineWidth(const char *text, unsigned fontSize) const {
            float fontScale = stbtt_ScaleForPixelHeight(&_self, float(fontSize));
            float resultWidth = 0.0f;
            unsigned tchLen = 0;

            for (const char *charPtr = text; charPtr[0] != 0 && charPtr[0] != '\n'; charPtr += tchLen) {
                unsigned short ch = string::utf8ToUTF16(charPtr, &tchLen);

                int glyph = stbtt_FindGlyphIndex(&_self, ch);
                int iadvance, ilsb;

                stbtt_GetGlyphHMetrics(&_self, glyph, &iadvance, &ilsb);

                resultWidth += std::floor(fontScale * float(ilsb));
                resultWidth += std::ceil(fontScale * float(iadvance) - fontScale * float(ilsb));
            }

            return resultWidth;
        }

        const FontResource::CharData *FontResource::_cacheChar(unsigned short ch, FontForm *fontForm) const {
            FontForm::Atlas *curAtlas = nullptr;
            CharData *curCharData = new CharData;

            int glyph = stbtt_FindGlyphIndex(&_self, ch);
            int ix0, ix1, iy0, iy1, iadvance, ilsb;

            stbtt_GetGlyphHMetrics(&_self, glyph, &iadvance, &ilsb);
            stbtt_GetGlyphBitmapBoxSubpixel(&_self, glyph, fontForm->fontScale, fontForm->fontScale, 0, 0, &ix0, &iy0, &ix1, &iy1);

            int dx = int(ix1 - ix0) + 2 * fontForm->blur + 1;
            int dy = int(iy1 - iy0) + 2 * fontForm->blur + 1;
            curAtlas = fontForm->atlasListEnd;

            if(curAtlas == nullptr){
                curAtlas = new FontForm::Atlas(*_log, *_api);
                fontForm->atlasListEnd = curAtlas;
                fontForm->atlasList = curAtlas;
            }
            else if(FONT_ATLAS_SIZE - curAtlas->hOffset - FONT_MIN_OFFSET <= dx){
                curAtlas->hOffset = FONT_MIN_OFFSET;
                curAtlas->vOffset += fontForm->height + 2 * fontForm->blur;

                if(FONT_ATLAS_SIZE - curAtlas->vOffset - FONT_MIN_OFFSET <= fontForm->height + 2 * fontForm->blur){
                    curAtlas->nextPtr = new FontForm::Atlas(*_log, *_api);
                    curAtlas = curAtlas->nextPtr;
                    fontForm->atlasListEnd = curAtlas;
                }
            }
            
            unsigned char glyphBufferSrc[2 * FONT_MAX_SIZE * FONT_MAX_SIZE] = {0};
            unsigned char glyphBufferTmp[2 * FONT_MAX_SIZE * FONT_MAX_SIZE] = {0};

            unsigned char *src = glyphBufferSrc + (fontForm->baseline + iy0 + FONT_MIN_OFFSET + fontForm->blur) * FONT_MAX_SIZE + FONT_MIN_OFFSET + fontForm->blur;
            unsigned char *tmp = glyphBufferTmp + (fontForm->baseline + iy0 + FONT_MIN_OFFSET + fontForm->blur) * FONT_MAX_SIZE + FONT_MIN_OFFSET + fontForm->blur;
            
            stbtt_MakeGlyphBitmapSubpixel(&_self, src + fontForm->blur * dx + fontForm->blur + 1, FONT_MAX_SIZE, FONT_MAX_SIZE, dx, fontForm->fontScale, fontForm->fontScale, 0, 0, glyph);
            
            if (fontForm->blur != 0) {
                _processGlyphGlow(src, tmp, dx, dy, fontForm->blur);
            }
            if (iy1 > iy0 && dx) {
                curAtlas->texture->update(0, curAtlas->hOffset, curAtlas->vOffset + fontForm->baseline + iy0, dx, dy, src);
            }
            
            curCharData->tu = float(curAtlas->hOffset) / float(FONT_ATLAS_SIZE);
            curCharData->tv = float(curAtlas->vOffset) / float(FONT_ATLAS_SIZE);
            curCharData->atlas = curAtlas;
            curCharData->txWidth = float(dx) / float(FONT_ATLAS_SIZE);
            curCharData->txHeight = float(fontForm->height + 2 * fontForm->blur) / float(FONT_ATLAS_SIZE); //
            curCharData->width = float(dx);
            curCharData->height = float(fontForm->height + 2 * fontForm->blur);
            curCharData->advance = fontForm->fontScale * float(iadvance);
            curCharData->lsb = fontForm->fontScale * float(ilsb);

            curAtlas->hOffset += dx + 1;
            fontForm->chars[ch] = curCharData;

            return curCharData;
        }

        // todo: optimize
        //
        void FontResource::_processGlyphGlow(unsigned char *src, unsigned char *tmp, int w, int h, unsigned blur) const {
            for (unsigned k = 0; k < blur; k++) {
                for (int i = 0; i < w; i++) {
                    for (int c = 0; c < h; c++) {
                        unsigned base = c * w + i;
                        unsigned ch = src[base];

                        ch += src[base + 1];
                        ch += src[base - 1];
                        ch += src[base + w];
                        ch += src[base - w];
                        ch += src[base + w + 1];
                        ch += src[base - w - 1];
                        ch += src[base + w - 1];
                        ch += src[base - w + 1];

                        src[base] = (unsigned char)(std::min(255u, ch >> 3));
                    }
                }
            }
        }

        FontResource::FontForm *FontResource::_cacheForm(unsigned fontSize, unsigned blur) const {
            FontForm *curFontForm = new FontForm();

            curFontForm->fontScale = stbtt_ScaleForPixelHeight(&_self, float(fontSize));
            curFontForm->height = fontSize;
            curFontForm->blur = (int)(blur);

            int ascent;
            stbtt_GetFontVMetrics(&_self, &ascent, 0, 0);
            curFontForm->baseline = (int)(ascent * curFontForm->fontScale);

            _fontForms[(fontSize << 16) + blur] = curFontForm;
            return curFontForm;
        }
    }
}




//