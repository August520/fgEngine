
namespace fg {
    unsigned string::utf8CharLen(const char *utf8Char) {
        size_t i = 0, j = 0;
        while(utf8Char[i]) {
            if((utf8Char[i] & 0xC0) != 0x80){
                if(++j > 1){
                    break;
                }
            }
            i++;
        }
        return i;
    }
    
    string::string() : _size(0), _str(_data.chars), _hash(0) {
        _data.chars[0] = 0;        
    }

    string::string(const char *utf8str, unsigned byteSize) {
        _size = byteSize;
        _allocate();
        _initmem(utf8str);
        _rehash();
    }
    
    string::string(const char *&utf8str) {
        _size = strlen(utf8str);
        _allocate();
        _initmem(utf8str);
        _rehash();
    }

    string::string(const string &str) {
        _size = str._size;
        _allocate();
        _initmem(str._str);
        _hash = str._hash;
    }

    string::string(string &&str) {
        _hash = str._hash; 
        _size = str._size;
        str._size = 0;

        if(_size <= CAPACITY) {
            memcpy(_data.chars, str._data.chars, _size + 1);
            _str = _data.chars;
        }
        else {
            _str = _data.ptr = str._data.ptr;            
        }
    }

    string::~string() {
        _free();
    }

    string &string::operator =(const char *utf8str) {
        _free();        
        _size = strlen(utf8str);        
        _allocate();
        _initmem(utf8str);
        _rehash();
        return *this;
    }

    string &string::operator =(const string &str) {
        _free();
        _size = str._size;
        _allocate();
        _initmem(str._str);        
        _hash = str._hash;
        return *this;
    }
    
    string &string::operator =(string &&str) {
        _free();

        _size = str._size;
        str._size = 0;

        if(_size <= CAPACITY) {
            memcpy(_data.chars, str._data.chars, _size + 1);
            _str = _data.chars;
        }
        else {
            _str = _data.ptr = str._data.ptr;
        }

        _hash = str._hash;
        return *this;
    }

    bool string::operator ==(const string &str) const {
        if(_size == str._size) {
            const   size_t *s1w = (const size_t *)_str;
            const   size_t *s2w = (const size_t *)str._str;

            if(*s1w == *s2w) {
                for(size_t i = 1; i < _size / sizeof(size_t) + 1; ++i) {
                    if(s1w[i] ^ s2w[i]) return false;
                }

                return true;
            }
        }
        return false;
    }

    bool string::operator !=(const string &str) const {
        return !operator ==(str);
    }

    void string::set(const char *utf8str, unsigned byteSize) {
        _size = byteSize;
        _allocate();
        _initmem(utf8str);
        _rehash();
    }

    const char *string::data() const {
        return _str;
    }

    unsigned short string::size() const {
        return _size;
    }

    size_t string::hash() const {
        return _hash;
    }

    bool string::empty() const {
        return _size == 0;
    }

    void string::_rebase() {
        _str = _size <= CAPACITY ? _data.chars : _data.ptr;
    }

    void string::_default() {
        _size = 0;
        _hash = 0;
        _str = _data.chars;
        _data.chars[0] = 0;
    }

    void string::_allocate() {
        if(_size <= CAPACITY) {
            *((size_t *)(_data.chars + _size)) = 0;
            _str = _data.chars;
        }
        else {
            _data.ptr = (char *)malloc(_size + sizeof(size_t));
            *((size_t *)(_data.ptr + _size)) = 0;
            _str = _data.ptr;
        }
    }

    void string::_free() {
        if(_size > CAPACITY) {
            free(_data.ptr);
        }
    }

    void string::_initmem(const char *src) {
        size_t i = 0;

        const size_t *source = (const size_t *)src;
        size_t *dest = (size_t *)_str;

        for(; i < _size / sizeof(size_t); ++i) {
            dest[i] = source[i];
        }

        size_t off = i * sizeof(size_t);
        const char *s = src + off;
        char *d = _str + off;

        while(off++ < _size) {
            *d++ = *s++;
        }
    }
    
    void string::_rehash() {
        size_t hash = 0;
        const size_t *s = (const size_t *)_str;

        for(size_t i = 0; i < _size / sizeof(size_t) + 1; ++i) {
            hash += s[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }

        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);

        _hash = hash;
    }

}



