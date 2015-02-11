
// todo:
// + safely

#include <unordered_map>

namespace fg {
    namespace resources {
        class FontResource : public FontResourceInterface, public Resource {
        public:
            static const int FONT_ATLAS_SIZE = 512;
            static const int FONT_MIN_SIZE = 6;
            static const int FONT_MAX_SIZE = 512;

            struct CharData;
            struct FontSize {
                struct Atlas {
                    platform::Texture2DInterface *texture;
                    
                    Atlas   *nextPtr;
                    int     hOffset;
                    int     vOffset;

                    Atlas(const diag::LogInterface &log, platform::PlatformInterface &api);
                    ~Atlas();
                };

                std::unordered_map <wchar_t, CharData *> chars;

                Atlas   *atlasList;
                Atlas   *atlasListEnd;
                int     baseline;
                int     height;
                float   fontScale;

                ~FontSize();
            };

            struct CharData {
                FontSize::Atlas *atlas;

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

            void  cache(const char *mbcharsz, unsigned fontSize) override;
            void  getChar(const char *mbChar, unsigned fontSize, FontCharInfo &out) const override;
            void  getChar(wchar_t ch, unsigned fontSize, FontCharInfo &out) const override;
            float getTextWidth(const char *text, unsigned fontSize) const override;

        protected:
            mutable std::unordered_map     <unsigned int, FontSize *> _fontSizes;
            mutable tools::stbtt_fontinfo  _self;            
            platform::PlatformInterface    *_api;
            const diag::LogInterface       *_log;

            const CharData  *_cacheChar(wchar_t ch, FontSize *fontSize) const;
            FontSize        *_cacheFontSize(unsigned int fontSize) const;
        };
    }
}

