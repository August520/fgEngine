
#include <unordered_map>

namespace fg {
    namespace resources {
        class FontResource : public FontResourceInterface, public Resource {
        public:
            struct CharData;
            struct FontForm {
                struct Atlas {
                    platform::Texture2DInterface *texture = nullptr;
                    
                    Atlas   *nextPtr = nullptr;
                    int     hOffset;
                    int     vOffset;

                    Atlas(const diag::LogInterface &log, platform::PlatformInterface &api);
                    ~Atlas();
                };

                std::unordered_map <unsigned short, CharData *> chars;

                Atlas   *atlasList = nullptr;
                Atlas   *atlasListEnd = nullptr;
                int     baseline;
                int     height;
                int     glow;
                int     shadowX;
                int     shadowY;
                float   fontScale;

                ~FontForm();
            };

            struct CharData {
                FontForm::Atlas *atlas;

                float   tu;
                float   tv;
                float   txWidth;                                           // ширина в текстуре (0..1)
                float   txHeight;                                          // высота в текстуре (0..1)
                float   width;                                             // ширина в пикселях
                float   height;                                            // высота в пикселях
                float   advance;                                           // смещение следующего символа от начала этого в пикселях
                float   lsb;                                               // смещение слева для символа
            };

            FontResource(const char *path, bool unloadable);
            ~FontResource() override;

            void  loaded(const diag::LogInterface &log) override;
            bool  constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void  unloaded() override;

            unsigned cache(const char *mbcharsz, unsigned fontSize, unsigned glow, int shadowX, int shadowY, unsigned maxCaching) override;
            void  getChar(const char *mbChar, unsigned fontSize, unsigned glow, int shadowX, int shadowY, FontCharInfo &out) const override;
            void  getChar(unsigned short ch, unsigned fontSize, unsigned glow, int shadowX, int shadowY, FontCharInfo &out) const override;
            float getTextWidth(const char *text, unsigned fontSize) const override;
            float getLineWidth(const char *text, unsigned fontSize) const override;

        protected:
            mutable std::unordered_map     <unsigned, FontForm *> _fontForms;
            mutable tools::stbtt_fontinfo  _self;            
            platform::PlatformInterface    *_api;
            const diag::LogInterface       *_log;

            const CharData  *_cacheChar(unsigned short ch, FontForm *fontForm) const;
            FontForm        *_cacheForm(unsigned fontSize, unsigned glow, int shadowX, int shadowY) const;

            void  _processGlyphGlow(unsigned char *src, unsigned char *tmp, int w, int h, unsigned glow, int shadowX, int shadowY) const;
        };
    }
}

