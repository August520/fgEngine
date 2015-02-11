
namespace fg {
    namespace resources {
        TextResource::~TextResource() {
            delete _text;
        }

        void TextResource::loaded(const diag::LogInterface &log) {
            _size = _binarySize;
            _text = new char [_size + 1];
            _text[_size] = 0;
            memcpy(_text, _binaryData, _size);
        }

        bool TextResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            return false;
        }

        void TextResource::unloaded() {
            delete _text;
            _text = nullptr;
            _size = 0;
        }

        const char *TextResource::getText() const {
            if(_text) return _text;
            else return "";
        }

        int TextResource::getSize() const {
            return _size;
        }
    }
}


