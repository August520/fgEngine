
//--- TODO: 

#pragma push_macro("new")
#undef new

#include <new>
#include <type_traits>
#include <unordered_map>

template <typename F> struct callback final {

};
template <typename RET, typename... ARG> struct callback <RET (ARG...)> final {
    static const unsigned _MTD_SIZE = 24;

    callback() : _callPtr(nullptr), _ptr(nullptr) {

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

struct byteform {
    byteform(void *itarget, unsigned int isize) : _target((char *)itarget), _offset(0), _size(isize) {}

    byteform &startOff(unsigned int startOffset) {
        _offset = startOffset;
        return *this;
    }

    unsigned int getSize() const {
        return _size;
    }

    unsigned int getOffset() const {
        return _offset;
    }

    void incOffset(unsigned int increment) const {
        _offset += increment;
    }

    const char *getPtr() const {
        return _target;
    }

    char *getCurrentPtr() const {
        return _target + _offset;
    }

    byteform &writeChar(char v) {
        *((char *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeByte(unsigned char v) {
        *((unsigned char *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeShort(short v) {
        *((short *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeWord(unsigned short v) {
        *((unsigned short *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeInt(int v) {
        *((int *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeDword(unsigned int v) {
        *((unsigned int *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeInt64(__int64 v) {
        *((__int64 *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeQword(unsigned __int64 v) {
        *((unsigned __int64 *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeFloat(float v) {
        *((float *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeDouble(double v) {
        *((double *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }
    byteform &writeBytes(const char *data, int count) {
        memcpy(_target + _offset, data, count);
        _offset += count;
        return *this;
    }
    byteform &writeString(const char *str) {
        unsigned short len = (unsigned short)strlen(str);
        writeWord(len);
        memcpy(_target + _offset, str, len);
        _offset += len;
        return *this;
    }
    template <typename T> byteform &write(const T &v) {
        *((T *)&_target[_offset]) = v;
        _offset += sizeof(v);
        return *this;
    }

    char readChar() const {
        char r = *((char *)&_target[_offset]);
        _offset += sizeof(char);
        return r;
    }
    unsigned char readByte() const {
        unsigned char r = *((unsigned char *)&_target[_offset]);
        _offset += sizeof(unsigned char);
        return r;
    }
    short readShort() const {
        short r = *((short *)&_target[_offset]);
        _offset += sizeof(short);
        return r;
    }
    unsigned short readWord() const {
        unsigned short r = *((unsigned short *)&_target[_offset]);
        _offset += sizeof(unsigned short);
        return r;
    }
    int readInt() const {
        int r = *((int *)&_target[_offset]);
        _offset += sizeof(int);
        return r;
    }
    unsigned int readDword() const {
        unsigned int r = *((unsigned int *)&_target[_offset]);
        _offset += sizeof(unsigned int);
        return r;
    }
    __int64 readInt64() const {
        __int64 r = *((__int64 *)&_target[_offset]);
        _offset += sizeof(__int64);
        return r;
    }
    unsigned __int64 readQword() const {
        unsigned __int64 r = *((unsigned __int64 *)&_target[_offset]);
        _offset += sizeof(unsigned __int64);
        return r;
    }
    float readFloat() const {
        float r = *((float *)&_target[_offset]);
        _offset += sizeof(float);
        return r;
    }
    double readDouble() const {
        double r = *((double *)&_target[_offset]);
        _offset += sizeof(double);
        return r;
    }
    const byteform &readBytes(char *odata, int count) const {
        memcpy(odata, _target + _offset, count);
        _offset += count;
        return *this;
    }
    const byteform &readString(char *ostr) const {
        unsigned short len = readWord();
        memcpy(ostr, _target + _offset, len);
        ostr[len] = 0;
        _offset += len;
        return *this;
    }
    template <typename T> const byteform &read(T &out) const {
        out = *((T *)&_target[_offset]);
        _offset += sizeof(T);
    }

    const byteform &readChar(char &out) const {
        out = *((char *)&_target[_offset]);
        _offset += sizeof(char);
        return *this;
    }
    const byteform &readByte(unsigned char &out) const {
        out = *((unsigned char *)&_target[_offset]);
        _offset += sizeof(unsigned char);
        return *this;
    }
    const byteform &readShort(short &out) const {
        out = *((short *)&_target[_offset]);
        _offset += sizeof(short);
        return *this;
    }
    const byteform &readWord(unsigned short &out) const {
        out = *((unsigned short *)&_target[_offset]);
        _offset += sizeof(unsigned short);
        return *this;
    }
    const byteform &readInt(int &out) const {
        out = *((int *)&_target[_offset]);
        _offset += sizeof(int);
        return *this;
    }
    const byteform &readDword(unsigned int &out) const {
        out = *((unsigned int *)&_target[_offset]);
        _offset += sizeof(unsigned int);
        return *this;
    }
    const byteform &readInt64(__int64 &out) const {
        out = *((__int64 *)&_target[_offset]);
        _offset += sizeof(__int64);
        return *this;
    }
    const byteform &readQword(unsigned __int64 &out) const {
        out = *((unsigned __int64 *)&_target[_offset]);
        _offset += sizeof(unsigned __int64);
        return *this;
    }
    const byteform &readFloat(float &out) const {
        out = *((float *)&_target[_offset]);
        _offset += sizeof(float);
        return *this;
    }
    const byteform &readDouble(double &out) const {
        out = *((double *)&_target[_offset]);
        _offset += sizeof(double);
        return *this;
    }

protected:
    char *_target;
    mutable unsigned int  _offset;
    unsigned int  _size;
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

    StateMachine() : _cur(nullptr), _curName((NAME_TYPE)-1) {}
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
        _curName = name;
    }

    bool stateEvent(const EVENT_TYPE &genEvent) {
        auto index = _cur->nexts.find(genEvent);

        if(index != _cur->nexts.end()) {
            StateNode *nst = _nodes[index->second];
            StateNode *lst = _cur;

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
    StateNode  *_cur;
};
