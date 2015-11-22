
namespace fg {
    namespace object2d {
        class TextField : public TextFieldInterface, public DisplayObject {
        public:
            TextField();
            ~TextField() override;

            void  appendText(const std::string &text) override;
            void  setText(const std::string &text) override;
            void  setFont(const fg::string &fontResourceName) override;
            void  setSize(unsigned size) override;
            void  setColor(const fg::color &c) override;
            void  setGlow(unsigned pixels) override;
            void  setOutlineColor(const fg::color &c) override;
            void  setShadow(int shadowX, int shadowY) override;
            void  setAlign(TextAlign align) override;

            const resources::FontResourceInterface *getFont() const override;
            const std::string &getText() const override;
            const FontForm &getForm() const override;
            TextAlign getAlign() const override;

            void  updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) override;
            bool  isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) override;

        protected:
            unsigned     _cached = 0;
            TextAlign    _align  = TextAlign::LEFT;
            fg::string   _fontResourcePath;
            std::string  _text;
            FontForm     _form;
            
            resources::FontResourceInterface  *_font = nullptr;
        };
    }
}
