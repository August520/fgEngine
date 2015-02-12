
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

                std::unordered_map <unsigned short, CharData *> chars;

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
                float   txWidth;                                           // ������ � �������� (0..1)
                float   txHeight;                                          // ������ � �������� (0..1)
                float   width;                                             // ������ � ��������
                float   height;                                            // ������ � ��������
                float   advance;                                           // �������� ���������� ������� �� ������ ����� � ��������
                float   lsb;                                               // �������� ����� ��� �������
            };

            FontResource(const char *path, bool unloadable);
            ~FontResource() override;

            void  loaded(const diag::LogInterface &log) override;
            bool  constructed(const diag::LogInterface &log, platform::PlatformInterface &api) override;
            void  unloaded() override;

            void  cache(const char *mbcharsz, unsigned fontSize) override;
            void  getChar(const char *mbChar, unsigned fontSize, FontCharInfo &out) const override;
            void  getChar(unsigned short ch, unsigned fontSize, FontCharInfo &out) const override;
            float getTextWidth(const char *text, unsigned fontSize) const override;

        protected:
            mutable std::unordered_map     <unsigned, FontSize *> _fontSizes;
            mutable tools::stbtt_fontinfo  _self;            
            platform::PlatformInterface    *_api;
            const diag::LogInterface       *_log;

            const CharData  *_cacheChar(unsigned short ch, FontSize *fontSize) const;
            FontSize        *_cacheFontSize(unsigned fontSize) const;
        };
    }
}

