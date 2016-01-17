
//--- TODO: 

#pragma push_macro("new")
#undef new

#include <cstring>

#include <new>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>

typedef long long int64;

template <typename F> struct callback final {

};
template <typename RET, typename... ARG> struct callback <RET (ARG...)> final {
    static const unsigned _MTD_SIZE = 24;

    callback() : _callPtr(nullptr), _ptr(nullptr) {

    }
    callback(const callback <RET(ARG...)> &cb) {
        if(cb._callPtr) {
            _callPtr = cb._callPtr;
            _ptr = cb._ptr;
            memcpy(_mtd, cb._mtd, _MTD_SIZE);
        }
        else {
            _callPtr = nullptr;
        }
    }
    callback(RET (*func)(ARG...)) : _callPtr(nullptr), _ptr(nullptr) {
        set(func);
    }
    template <class C> callback(C *obj, RET (C::*method)(ARG...)) : _callPtr(nullptr), _ptr(nullptr) {
        set(obj, method);
    }
    template <class F> callback(F lambda) : _callPtr(nullptr), _ptr(nullptr) {
        set(lambda);
    }

    void set(RET (*func)(ARG...)) {
        _ptr = (void *)func;
        _callPtr = (void *)_cbHelperFunction::call;
    }
    template <class C> void set(C * obj, RET (C::*method)(ARG...)) {
        typedef RET (C::*FuncType)(ARG...);
        *(FuncType *)_mtd = method;
        _callPtr = (void *)_cbHelperMethod <C>::call;
        _ptr = obj;
    }
    template <class F> void set(F funcObj) {
        static_assert(sizeof(F) < _MTD_SIZE, "lambda size very large");
        new (_mtd) F (funcObj);
        _callPtr = (void *)_cbHelperLambda <F>::call;
    }
    void set(std::nullptr_t) {
        _callPtr = nullptr;
    }

    bool operator ==(const callback <RET (ARG...)> &cb) {
        return _callPtr == cb._callPtr && _ptr == cb._ptr;
    }
    void operator =(const callback <RET(ARG...)> &cb) {
        if(cb._callPtr) {
            _callPtr = cb._callPtr;
            _ptr = cb._ptr;
            memcpy(_mtd, cb._mtd, _MTD_SIZE);
        }
        else {
            _callPtr = nullptr;
        }
    }

    RET operator ()(ARG... args) {
        typedef RET (*HelperFunc)(callback <RET (ARG...)> *cb, ARG...);
        HelperFunc f = (HelperFunc)_callPtr;
        return f(this, args...);
    }

    bool isBinded() {
        return _callPtr != nullptr;
    }

protected:
    template <class C> struct _cbHelperMethod {
        static RET call(callback <RET (ARG...)> *cb, ARG... args) {
            typedef RET (C::*FuncType)(ARG...);
            C *targetObj = (C *)cb->_ptr;
            FuncType targetMethod = *(FuncType *)cb->_mtd;
            return (targetObj->*targetMethod)(args...);
        }
    };
    template <class F> struct _cbHelperLambda {
        static RET call(callback <RET (ARG...)> *cb, ARG... args) {
            return (*(F *)cb->_mtd)(args...);
        }
    };
    struct _cbHelperFunction {
        static RET call(callback <RET (ARG...)> *cb, ARG... args) {
            typedef RET (*FuncType)(ARG...);
            FuncType targetFunction = (FuncType)cb->_ptr;
            return targetFunction(args...);
        }
    };

    char  _mtd[_MTD_SIZE];
    void  *_ptr;
    void  *_callPtr;
};

#pragma pop_macro("new")

template <class C, typename RET, typename... ARG> callback <RET(ARG...)> make_callback(C *obj, RET(C::*method)(ARG...)) {
    return callback<RET(ARG...)>(obj, method);
}

//--- TODO: 

struct byteinput {
    byteinput(void *target, unsigned size) : _target((char *)target), _offset(0), _size(size) {

    }
    byteinput &startOff(unsigned int startOffset) {
        _offset = startOffset;
        return *this;
    }
    const byteinput &startOff(unsigned int startOffset) const {
        _offset = startOffset;
        return *this;
    }
    unsigned getSize() const {
        return _size;
    }
    unsigned getOffset() const {
        return _offset;
    }
    const char *getPtr() const {
        return _target;
    }
    char *getPtr() {
        return _target;
    }
    const char *getCurrentPtr() const {
        return _target + _offset;
    }
    char readChar() const {
        if (_offset <= _size - sizeof(char)) {
            char r = *((char *)&_target[_offset]);
            _offset += sizeof(char);
            return r;
        }
        else {
            return 0;
        }
    }
    unsigned char readByte() const {
        if (_offset <= _size - sizeof(unsigned char)) {
            unsigned char r = *((unsigned char *)&_target[_offset]);
            _offset += sizeof(unsigned char);
            return r;
        }
        else {
            return 0;
        }
    }
    short readShort() const {
        if (_offset <= _size - sizeof(short)) {
            short r = *((short *)&_target[_offset]);
            _offset += sizeof(short);
            return r;
        }
        else {
            return 0;
        }
    }
    unsigned short readWord() const {
        if (_offset <= _size - sizeof(unsigned short)) {
            unsigned short r = *((unsigned short *)&_target[_offset]);
            _offset += sizeof(unsigned short);
            return r;
        }
        else {
            return 0;
        }
    }
    int readInt() const {
        if (_offset <= _size - sizeof(int)) {
            int r = *((int *)&_target[_offset]);
            _offset += sizeof(int);
            return r;
        }
        else {
            return 0;
        }
    }
    unsigned int readDword() const {
        if (_offset <= _size - sizeof(unsigned int)) {
            unsigned int r = *((unsigned int *)&_target[_offset]);
            _offset += sizeof(unsigned int);
            return r;
        }
        else {
            return 0;
        }
    }
    int64 readInt64() const {
        if (_offset <= _size - sizeof(int64)) {
            int64 r = *((int64 *)&_target[_offset]);
            _offset += sizeof(int64);
            return r;
        }
        else {
            return 0;
        }
    }
    unsigned long long readQword() const {
        if (_offset <= _size - sizeof(unsigned long long)) {
            unsigned long long r = *((unsigned long long *)&_target[_offset]);
            _offset += sizeof(unsigned long long);
            return r;
        }
        else {
            return 0;
        }
    }
    float readFloat() const {
        if (_offset <= _size - sizeof(float)) {
            float r = *((float *)&_target[_offset]);
            _offset += sizeof(float);
            return r;
        }
        else {
            return 0.0f;
        }
    }
    double readDouble() const {
        if (_offset <= _size - sizeof(double)) {
            double r = *((double *)&_target[_offset]);
            _offset += sizeof(double);
            return r;
        }
        else {
            return 0.0;
        }
    }
    const byteinput &readBytes(char *odata, int count) const {
        if (_offset <= _size - count) {
            memcpy(odata, _target + _offset, count);
            _offset += count;
        }
        return *this;
    }
    const byteinput &readString(char *ostr, unsigned maxlen) const {
        unsigned len = 0;
        if (_offset <= _size - sizeof(unsigned short)) {
            len = readWord();
            len = len < maxlen ? len : maxlen;
        }
        if (_offset <= _size - len) {
            memcpy(ostr, _target + _offset, len);
            ostr[len] = 0;
            _offset += len;
        }
        return *this;
    }
    template <typename T> const byteinput &read(T &out) const {
        if (_offset <= _size - sizeof(T)) {
            out = *((T *)&_target[_offset]);
            _offset += sizeof(T);
        }
        return *this;
    }

protected:
    char *_target;
    mutable unsigned _offset;
    unsigned _size;
};

struct byteoutput {
    byteoutput(void *target, unsigned maxsize) : _target((char *)target), _offset(0), _maxsize(maxsize) {

    }
    byteoutput &startOff(unsigned startOffset) {
        _offset = startOffset;
        return *this;
    }
    unsigned getOffset() const {
        return _offset;
    }
    const char *getPtr() const {
        return _target;
    }
    char *getPtr() {
        return _target;
    }
    char *getCurrentPtr() {
        return _target + _offset;
    }
    byteoutput &writeChar(char v) {
        if (_offset < _maxsize - sizeof(char)) {
            *((char *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeByte(unsigned char v) {
        if (_offset < _maxsize - sizeof(unsigned char)) {
            *((unsigned char *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeShort(short v) {
        if (_offset < _maxsize - sizeof(short)) {
            *((short *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeWord(unsigned short v) {
        if (_offset < _maxsize - sizeof(unsigned short)) {
            *((unsigned short *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeInt(int v) {
        if (_offset < _maxsize - sizeof(int)) {
            *((int *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeDword(unsigned int v) {
        if (_offset < _maxsize - sizeof(unsigned int)) {
            *((unsigned int *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeInt64(int64 v) {
        if (_offset < _maxsize - sizeof(int64)) {
            *((int64 *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeQword(unsigned long long v) {
        if (_offset < _maxsize - sizeof(unsigned long long)) {
            *((unsigned long long *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeFloat(float v) {
        if (_offset < _maxsize - sizeof(float)) {
            *((float *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeDouble(double v) {
        if (_offset < _maxsize - sizeof(double)) {
            *((double *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }
    byteoutput &writeBytes(const char *data, int count) {
        if (_offset < _maxsize - count) {
            memcpy(_target + _offset, data, count);
            _offset += count;
        }
        return *this;
    }
    byteoutput &writeString(const char *str) {
        unsigned short len = (unsigned short)strlen(str);
        if (_offset < _maxsize - sizeof(unsigned short) - len) {
            writeWord(len);
            memcpy(_target + _offset, str, len);
            _offset += len;
        }
        return *this;
    }
    template <typename T> byteoutput &write(const T &v) {
        if (_offset < _maxsize - sizeof(T)) {
            *((T *)&_target[_offset]) = v;
            _offset += sizeof(v);
        }
        return *this;
    }

protected:
    char *_target;
    unsigned _offset;
    unsigned _maxsize;
};

//---

struct uncopyable {
    uncopyable() {}
    virtual ~uncopyable() {}
    uncopyable(const uncopyable &) = delete;
    uncopyable &operator =(const uncopyable &) = delete;
};

//---

template <class USER_OBJ, typename EVENT_TYPE, typename NAME_TYPE> class StateMachine {
public:
    struct StateNode : public USER_OBJ {
        std::unordered_map  <EVENT_TYPE, NAME_TYPE> nexts;
        
        callback  <void ()> entryHandler;
        callback  <void ()> leaveHandler;
        callback  <void (float)> updateHandler;
    };

    StateMachine() : _cur(nullptr), _curName((NAME_TYPE)-1), _prevName((NAME_TYPE)-1) {}
    ~StateMachine() {
        for(auto index = _nodes.begin(); index != _nodes.end(); index++) {
            StateNode *t = index->second;
            delete t;
        }
    }

    StateNode &addStateNode(const NAME_TYPE &name) {
        auto index = _nodes.find(name);
        StateNode *tnode;

        if(index == _nodes.end()) {
            tnode = new StateNode();
            _nodes[name] = tnode;

            if(_cur == nullptr) {
                _cur = tnode;
                _curName = name;
            }
            return *tnode;
        }
        else {
            return *index->second;
        }
    }

    void setStateNode(const NAME_TYPE &name) {
        _cur = _nodes[name];
        _prevName = _curName;
        _curName = name;
    }

    bool stateEvent(const EVENT_TYPE &genEvent) {
        auto index = _cur->nexts.find(genEvent);

        if(index != _cur->nexts.end()) {
            StateNode *nst = _nodes[index->second];
            StateNode *lst = _cur;

            _prevName = _curName;
            _curName = index->second;
            _cur = nst;

            if(lst->leaveHandler.isBinded()) {
                lst->leaveHandler();
            }

            if(nst->entryHandler.isBinded()) {
                nst->entryHandler();
            }
            return true;
        }
        return false;
    }

    const NAME_TYPE &getCurrentStateName() const {
        return _curName;
    }
    const NAME_TYPE &getPreviousStateName() const {
        return _prevName;
    }
    const StateNode &getCurrentStateNode() const {
        return *_cur;
    }

    void update(float frameTimeMs) {
        if(_cur && _cur->updateHandler.isBinded()) {
            _cur->updateHandler(frameTimeMs);
        }
    }

protected:
    std::unordered_map  <NAME_TYPE, StateNode *> _nodes;

    NAME_TYPE  _curName;
    NAME_TYPE  _prevName;
    StateNode  *_cur;
};
