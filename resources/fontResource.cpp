
//TODO: margin, safe, un-winapi, optimization

namespace fg {
    namespace resources {
        FontResource::FontSize::Atlas::Atlas(const diag::LogInterface &log, platform::PlatformInterface &api) : hOffset(0), vOffset(0), nextPtr(nullptr) {
            texture = api.rdCreateTexture2D(platform::TextureFormat::ALPHA8, FONT_ATLAS_SIZE, FONT_ATLAS_SIZE, 1);

            if(texture == nullptr){
                log.msgError("FontResource::Atlas::Atlas can't create texture");
            }
        }

        FontResource::FontSize::Atlas::~Atlas() {
            if(texture) {
                texture->release();
            }
        }

        FontResource::FontSize::~FontSize() {
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
            for(auto index = _fontSizes.begin(); index != _fontSizes.end(); ++index){
                delete index->second;
            }
        }

        void FontResource::loaded(const diag::LogInterface &log) {
            if(!stbtt_InitFont(&_self, (unsigned char *)_binaryData, 0)){
                log.msgError("ResFont::loaded %s bad format", _loadPath);
                _state = ResourceState::INVALID;
            }
        }

        bool FontResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            _api = &api;
            _log = &log;
            return true; // keep binary in memory
        }

        void FontResource::unloaded() {
            for(auto index = _fontSizes.begin(); index != _fontSizes.end(); ++index){
                delete index->second;
            }

            _fontSizes.clear();

            delete[] _binaryData;
            _binaryData = nullptr;
            _binarySize = 0;
        }

        void FontResource::cache(const char *mbcharsz, unsigned fontSize) {
            if(fontSize < FONT_MIN_SIZE || fontSize > FONT_MAX_SIZE) return;

            FontSize *curFontSize = nullptr;
            auto fontSzIndex = _fontSizes.find(fontSize);

            if(fontSzIndex != _fontSizes.end()){
                curFontSize = fontSzIndex->second;
            }
            else{
                curFontSize = _cacheFontSize(fontSize);
            }

            wchar_t  ch = 0;
            unsigned chLen = 0;

            for(const char *charPtr = mbcharsz; charPtr[0] != 0; charPtr += chLen){
                chLen = mblen(charPtr, 6);
                mbtowc(&ch, charPtr, chLen);

                auto charIndex = curFontSize->chars.find(ch);
                if(charIndex == curFontSize->chars.end()){
                    _cacheChar(ch, curFontSize);
                }
            }
        }

        void FontResource::getChar(const char *mbChar, unsigned fontSize, FontCharInfo &out) const {
            wchar_t ch = 0;
            MultiByteToWideChar(CP_UTF8, 0, mbChar, 4, &ch, 1);
            getChar(ch, fontSize, out);
        }

        void FontResource::getChar(wchar_t ch, unsigned fontSize, FontCharInfo &out) const {
            FontSize        *curFontSize = nullptr;
            const CharData  *charData = nullptr;
            auto  fontSzIndex = _fontSizes.find(fontSize);

            if(fontSzIndex != _fontSizes.end()){
                curFontSize = fontSzIndex->second;
            }
            else{
                curFontSize = _cacheFontSize(fontSize);
            }
            auto charIndex = curFontSize->chars.find(ch);

            if(charIndex == curFontSize->chars.end()){
                charData = _cacheChar(ch, curFontSize);
            }
            else{
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
            float    fontScale = stbtt_ScaleForPixelHeight(&_self, float(fontSize));
            float    resultWidth = 0.0f;
            
            unsigned tchLen = 0;
            wchar_t  ch = 0;

            for(const char *charPtr = text; charPtr[0] != 0; charPtr += tchLen){
                tchLen = fg::string::utf8CharLen(charPtr);
                MultiByteToWideChar(CP_UTF8, 0, charPtr, 4, &ch, 1);

                int glyph = stbtt_FindGlyphIndex(&_self, ch);
                int iadvance, ilsb; 

                stbtt_GetGlyphHMetrics(&_self, glyph, &iadvance, &ilsb);

                resultWidth += floor(fontScale * float(ilsb));
                resultWidth += floor(fontScale * float(iadvance) - fontScale * float(ilsb));
            }

            return resultWidth;
        }

        const FontResource::CharData *FontResource::_cacheChar(wchar_t ch, FontSize *fontSize) const {
            FontSize::Atlas *curAtlas = nullptr;
            CharData *curCharData = new CharData;

            int glyph = stbtt_FindGlyphIndex(&_self, ch);
            int ix0, ix1, iy0, iy1, iadvance, ilsb;

            stbtt_GetGlyphHMetrics(&_self, glyph, &iadvance, &ilsb);
            stbtt_GetGlyphBitmapBoxSubpixel(&_self, glyph, fontSize->fontScale, fontSize->fontScale, 0, 0, &ix0, &iy0, &ix1, &iy1);
            int dx = int(ix1 - ix0);
            curAtlas = fontSize->atlasListEnd;

            if(curAtlas == nullptr){
                curAtlas = new FontSize::Atlas(*_log, *_api);
                fontSize->atlasListEnd = curAtlas;
                fontSize->atlasList = curAtlas;
            }
            else if(FONT_ATLAS_SIZE - curAtlas->hOffset < dx){ // - 10
                curAtlas->hOffset = 0;
                curAtlas->vOffset += fontSize->height; // + 10

                if(FONT_ATLAS_SIZE - curAtlas->vOffset < fontSize->height){ // - 10
                    curAtlas->nextPtr = new FontSize::Atlas(*_log, *_api);
                    curAtlas = curAtlas->nextPtr;
                    fontSize->atlasListEnd = curAtlas;
                }
            }

            unsigned char glyphBuffer[FONT_MAX_SIZE * FONT_MAX_SIZE] = {0};
            unsigned char *toff = glyphBuffer + (fontSize->baseline + iy0) * FONT_MAX_SIZE; // + 10  + 10
            stbtt_MakeGlyphBitmapSubpixel(&_self, toff, dx, iy1 - iy0, dx, fontSize->fontScale, fontSize->fontScale, 0, 0, glyph, ix0, iy0);
            
            curAtlas->texture->update(0, curAtlas->hOffset, curAtlas->vOffset + fontSize->baseline + iy0, dx, iy1 - iy0, toff); //??

            curCharData->tu = float(curAtlas->hOffset) / float(FONT_ATLAS_SIZE);
            curCharData->tv = float(curAtlas->vOffset) / float(FONT_ATLAS_SIZE);
            curCharData->atlas = curAtlas;
            curCharData->txWidth = float(dx) / float(FONT_ATLAS_SIZE);
            curCharData->txHeight = float(fontSize->height) / float(FONT_ATLAS_SIZE);
            curCharData->width = float(dx);
            curCharData->height = float(fontSize->height);
            curCharData->advance = fontSize->fontScale * float(iadvance);
            curCharData->lsb = fontSize->fontScale * float(ilsb);

            curAtlas->hOffset += dx + 1;
            fontSize->chars[ch] = curCharData;

            return curCharData;
        }

        FontResource::FontSize *FontResource::_cacheFontSize(unsigned int fontSize) const {
            FontSize *curFontSize = new FontSize();

            curFontSize->atlasList = nullptr;
            curFontSize->atlasListEnd = nullptr;
            curFontSize->fontScale = stbtt_ScaleForPixelHeight(&_self, float(fontSize));
            curFontSize->height = fontSize;

            int ascent;
            stbtt_GetFontVMetrics(&_self, &ascent, 0, 0);
            curFontSize->baseline = (int)(ascent * curFontSize->fontScale);

            _fontSizes[fontSize] = curFontSize;
            return curFontSize;
        }
    }
}




//