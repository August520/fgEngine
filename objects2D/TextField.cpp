
namespace fg {
    namespace object2d {
        TextField::TextField() {
            _type = DisplayObjectType::TEXTFIELD;
        }

        TextField::~TextField() {

        }

        void TextField::appendText(const std::string &text) {
            _text.append(text);
        }

        void TextField::setText(const std::string &text) {
            _text = text;
            _cached = 0;
        }
        void TextField::setFont(const fg::string &fontResourceName) {
            _fontResourcePath = fontResourceName;
        }

        void TextField::setSize(unsigned size) {
            _form.size = size;
        }

        void TextField::setColor(const fg::color &c) {
            _form.rgba = c;
        }

        void TextField::setGlow(unsigned pixels) {
            _form.glow = pixels;
            _cached = 0;
        }

        void TextField::setShadow(int shadowX, int shadowY) {
            _form.shadowX = shadowX;
            _form.shadowY = shadowY;
            _cached = 0;
        }

        void TextField::setAlign(TextAlign align) {
            _align = align;
        }

        const resources::FontResourceInterface *TextField::getFont() const {
            return _font;
        }

        const std::string &TextField::getText() const {
            return _text;
        }

        const FontForm &TextField::getForm() const {
            return _form;
        }

        TextAlign TextField::getAlign() const {
            return _align;
        }

        void TextField::updateCoordinates(float frameTimeMs, resources::ResourceManagerInterface &resMan) {
            DisplayObject::updateCoordinates(frameTimeMs, resMan);
        }

        bool TextField::isResourcesReady(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) {
            if (_font != nullptr) {
                if (_cached < unsigned(_text.size())) {                    
                    _cached += _font->cache(_text.c_str() + _cached, _form.size, _form.glow, _form.shadowX, _form.shadowY, 2);
                }
                else return true;
            }
            else if(_fontResourcePath.size()) {
                _font = resMan.getResource(_fontResourcePath);
            }
            
            return false;
        }

    }
}