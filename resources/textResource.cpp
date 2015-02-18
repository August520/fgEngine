
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
        
        void TextResource::save(void **outBinary, unsigned *outSize) const {
            *outBinary = new char [_size];
            *outSize = _size;
            memcpy(*outBinary, _text, _size);            
        }
        
        void TextResource::setText(const char *txt, unsigned size) {
            delete _text;

            _size = size;
            _text = new char [_size + 1];
            _text[_size] = 0;
            memcpy(_text, txt, _size);
        }

        bool TextResource::commit() {
            if(_savingState == ResourceSavingState::SAVED) {
                _savingState = ResourceSavingState::NEEDSAVE;
                return true;
            }
            else {
                return false;
            }
        }

        const char *TextResource::getText() const {
            if(_text) return _text;
            else return "";
        }

        unsigned TextResource::getSize() const {
            return _size;
        }
    }
}


