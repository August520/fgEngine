// TODO: 
//

#include <string.h>

namespace fg {
    class string final {
    public:
        static unsigned utf8CharLen(const char *utf8Char);
        static unsigned utf8ToUTF16(const char *utf8Char, unsigned *utf8Len = nullptr);

        static const unsigned short CAPACITY = 32;

        string();
        string(const char *utf8str, unsigned byteSize);
        string(const char *&utf8str);
        string(const string &str);
        string(string &&str);
        ~string();

        template <unsigned short SZ> string(const char (&utf8str)[SZ]) : _size(SZ) {
            _size = SZ - 1;
            _allocate();
            _initmem(utf8str);
            _rehash();
        }
        template <unsigned short SZ> string(char (&utf8str)[SZ]) : _size(SZ) {
            _size = (unsigned short)(strlen(utf8str));
            _allocate();
            _initmem(utf8str);
            _rehash();
        }

        string &operator =(const char *utf8str);
        string &operator =(const string &str);
        string &operator =(string &&str);

        template <unsigned short SZ> string &operator =(const char (&utf8str)[SZ]) {
            _free();
            _size = SZ - 1;
            _allocate();
            _initmem(utf8str);
            _rehash();
            return *this;
        }
        template <unsigned short SZ> string &operator =(char (&utf8str)[SZ]) {
            _free();
            _size = strlen(utf8str);
            _allocate();
            _initmem(utf8str);
            _rehash();
            return *this;
        }

        bool operator ==(const string &str) const;
        bool operator !=(const string &str) const;

        void set(const char *utf8str, unsigned byteSize);

        const char     *data() const;
        unsigned short  size() const;
        
        size_t hash()  const;
        bool   empty() const;

        void   _rebase();  // update str pointer
        void   _default(); // default constructor

    private:
        unsigned short  _size;
        
        size_t  _hash;
        char    *_str;
        
        union {
            char  chars[CAPACITY + 4];
            char  *ptr;         
        } _data;
        
        void _allocate();
        void _free();
        void _initmem(const char *src);
        void _rehash();
    };

    //---

    template <unsigned MAXELEMENTS, typename PTRTYPE> class StaticHash {
    public:
        StaticHash() : _entryIndex(0), _ptrIndex(0) {
            for(unsigned i = 0; i < ARRAYMAX; i++) {
                _data[i].count = 0;
                _data[i].keys = nullptr;
                _data[i].ptrs = nullptr;
            }
        }
        ~StaticHash() {
            clear();
        }

        void clear() {
            for(unsigned i = 0; i < ARRAYMAX; i++) {
                for(unsigned c = 0; c < _data[i].count; c++) {
                    _data[i].keys[c].~string();
                }

                free(_data[i].keys);
                free(_data[i].ptrs);

                _data[i].count = 0;
                _data[i].keys = nullptr;
                _data[i].ptrs = nullptr;
            }
        }

        // TODO: 'overriding' flag working
        //
        bool add(const fg::string &keystr, PTRTYPE ptr, bool overriding = false) {
            size_t origin = keystr.hash() & (ARRAYMAX - 1);
            Entry  &cur = _data[origin];

            cur.keys = (fg::string *)realloc(cur.keys, (cur.count + 1) * sizeof(fg::string));
            cur.ptrs = (PTRTYPE *)realloc(cur.ptrs, (cur.count + 1) * sizeof(PTRTYPE));

            for(unsigned i = 0; i < cur.count; i++) {
                cur.keys[i]._rebase();
            }

            cur.keys[cur.count]._default();
            cur.keys[cur.count] = keystr;
            cur.ptrs[cur.count++] = ptr;

            return true;
        }

        PTRTYPE get(const fg::string &keystr) const {
            size_t origin = keystr.hash() & (ARRAYMAX - 1);

            const fg::string *keys = _data[origin].keys;
            PTRTYPE const    *ptrs = _data[origin].ptrs;

            for(size_t i = 0, cnti = _data[origin].count; i < cnti; ++i) {
                if(keys[i] == keystr) {
                    return ptrs[i];
                }
            }

            return nullptr;
        }

        bool tryGet(const fg::string &keystr, PTRTYPE &out) const {
            size_t origin = keystr.hash() & (ARRAYMAX - 1);

            const fg::string *keys = _data[origin].keys;
            PTRTYPE const    *ptrs = _data[origin].ptrs;

            for(size_t i = 0, cnti = _data[origin].count; i < cnti; ++i) {
                if(keys[i] == keystr) {
                    out = ptrs[i];
                    return true;
                }
            }

            return false;
        }

        // input lambda must have signature: [](const fg::string &, PTRTYPE){}
        //
        template <typename LAMBDA> void foreach(LAMBDA func) {
            for(unsigned i = 0; i < ARRAYMAX; i++) {
                for(unsigned c = 0; c < _data[i].count; c++) {
                    func(_data[i].keys[c], _data[i].ptrs[c]);
                }
            }
        }

        // input lambda must have signature: [](const fg::string &, PTRTYPE){}
        //
        template <typename LAMBDA> void foreach(LAMBDA func) const {
            for(unsigned i = 0; i < ARRAYMAX; i++) {
                for(unsigned c = 0; c < _data[i].count; c++) {
                    func(_data[i].keys[c], _data[i].ptrs[c]);
                }
            }
        }

        PTRTYPE nextItem() {
            while(_ptrIndex < _data[_entryIndex].count) {
                return _data[_entryIndex].ptrs[_ptrIndex++];
            }

            _ptrIndex = 0;

            for(unsigned i = 0; i < ARRAYMAX / 2; i++) {
                _entryIndex = ++_entryIndex & (ARRAYMAX - 1);

                if(_data[_entryIndex].count != 0) {
                    return _data[_entryIndex].ptrs[_ptrIndex++];
                }
            }
            return nullptr;
        }

    protected:
        template <bool, unsigned num, unsigned power> struct _nearest_power_if {
            static const unsigned value = power;
        };
        template <unsigned num, unsigned power> struct _nearest_power_if <false, num, power> {
            static const unsigned value = _nearest_power_if <num <= power * 2, num, power << 1> ::value;
        };
        template <unsigned N> struct _nearest_power {
            static const unsigned value = _nearest_power_if <false, N, 1> ::value;
        };

        static const unsigned ARRAYMAX = _nearest_power <MAXELEMENTS> ::value * 2;

        struct Entry {
            unsigned       count;
            fg::string     *keys;
            PTRTYPE        *ptrs;
        };

        Entry     _data[ARRAYMAX];
        unsigned  _entryIndex;
        unsigned  _ptrIndex;
    };

}

namespace std {
    template <> struct hash <fg::string> {
        std::size_t operator ()(const fg::string &key) const {
            return key.hash();
        }
    };
}