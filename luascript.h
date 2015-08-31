// todo: luaObj optimize -> dynamic, lua namespace, variadic templates

#pragma once

#include "lua-5.1.4/lua.h"
#include "lua-5.1.4/lstate.h"
#include "lua-5.1.4/lualib.h"
#include "lua-5.1.4/lauxlib.h"

#include <string.h>
#include <unordered_map>

enum LUATYPE {
    LUATYPE_NULL = LUA_TNIL,
    LUATYPE_NUMBER = LUA_TNUMBER,
    LUATYPE_STRING = LUA_TSTRING,
    LUATYPE_BOOL = LUA_TBOOLEAN,
    LUATYPE_TABLE = LUA_TTABLE,
    LUATYPE_USERDATA = LUA_TUSERDATA,
    LUATYPE_CFUNC = LUA_TFUNCTION,
};

#define LUAOBJ_STRMAX 32
#define LUAOBJ_EPSILON 0.000001

struct byteform;
struct luaScript;

struct luaObj;
extern luaObj _luaObj_empty;


namespace std {
    template <> struct hash<luaObj> {
        std::size_t operator()(const luaObj& t) const;
    };
}

struct luaObj {
    typedef std::unordered_map <luaObj, luaObj *> hashMap;
    LUATYPE _type;

    struct _stdtypes{
        double  num;
        bool    boolean;
        void    *ptr;
        char    meta[LUAOBJ_STRMAX];

        _stdtypes(){
            meta[0] = 0;
        }
    } _std;

    struct _string{
        char         buf[LUAOBJ_STRMAX];
        char         *farptr;
        unsigned int sz;

        _string() : farptr(nullptr) {}
    } mutable _str;

    hashMap *_table;

    luaObj(void) : _type(LUATYPE_NULL), _table(nullptr){}
    luaObj(const char val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const unsigned char val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const short val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const unsigned short val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const int val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const unsigned int val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const long val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const unsigned long val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const float val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = (double)val;
    }
    luaObj(const double val) : _type(LUATYPE_NUMBER), _table(nullptr){
        _std.num = val;
    }
    luaObj(const bool val) : _type(LUATYPE_BOOL), _table(nullptr){
        _std.boolean = val;
    }

    luaObj(const char *str);

    luaObj(void *ptr) : _type(LUATYPE_USERDATA), _table(nullptr){
        _std.ptr = ptr;
    }
    luaObj(void *ptr, const char *meta) : _type(LUATYPE_USERDATA), _table(nullptr){
        _std.ptr = ptr;
        strcpy(_std.meta, meta);
    }
    luaObj(lua_State *luaState, int idx) : _type(LUATYPE_NULL), _table(nullptr){
        fromLuaStack(luaState, idx);
    }
    luaObj(const luaObj &copy);
    luaObj(luaObj &&from);

    ~luaObj();

    operator char() const;
    operator unsigned char() const;
    operator short() const;
    operator unsigned short() const;
    operator int() const;
    operator unsigned int() const;
    operator float() const;
    operator double() const;
    operator const char * () const;
    operator bool() const;
    operator void * () const;

    luaObj &operator = (const luaObj &copy);
    luaObj &operator = (luaObj &&rVal);
    bool    operator == (const luaObj &rVal) const;
    bool    operator < (const luaObj &rVal) const;

    LUATYPE type() const{                                                    
        return _type;
    }

    const char *getMeta() const{
        return _std.meta;
    }

    luaObj &add(const luaObj &key, const luaObj &value = luaObj());          // return reference to new element
    luaObj &add(luaObj &&key, luaObj &&value);
    luaObj &add(const luaObj &key, luaObj &&value);
    luaObj &add(luaObj &&key, const luaObj &value);

    luaObj &push(const luaObj &key, const luaObj &value);                    // return this 
    luaObj &push(luaObj &&key, luaObj &&value);
    luaObj &push(const luaObj &key, luaObj &&value);
    luaObj &push(luaObj &&key, const luaObj &value);

    luaObj &get(const luaObj &key) const;                            // return reference to table value or null obj
    luaObj *getptr(const luaObj &key);                               // return pointer to table value or nullptr

    unsigned int count() const;                                              // size of table

    void clear();
    bool remove(const luaObj &key);

    template <typename F> void  foreach(F functor){                          // functor signature: bool functor(const luaObj &key, luaObj &value);
        if(_type == LUATYPE_TABLE){
            for(hashMap::iterator index = _table->begin(); index != _table->end(); ++index){
                if(functor(index->first, *(index->second)) == false) break;
            }
        }
    }

    template <typename F> void  foreach(F functor) const{                    // functor signature: bool functor(const luaObj &key, luaObj &value);
        if(_type == LUATYPE_TABLE){
            for(hashMap::iterator index = _table->begin(); index != _table->end(); ++index){
                if(functor(index->first, *(index->second)) == false) break;
            }
        }
    }

    void  toLuaStack(lua_State *L) const;                                    
    void  fromLuaStack(lua_State *L, int idx);                               

    int   tableExport(unsigned char *oBin);                                  // export to binary
    void  tableImport(unsigned char *iBin, int iBinSize);                    // import from binary

    void  tableImport(const byteform & target);                              // import from byteform, TODO: format changes?

};

template <typename T> struct lsFunctionBinder {
    template <typename P1, typename P2, typename P3, typename P4> static int lsFunc(lua_State *L){
        luaObj param[4];
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop, 4);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        lua_pushvalue(L, lua_upvalueindex(1));
        T(*F)(P1, P2, P3, P4) = (T(*)(P1, P2, P3, P4))lua_touserdata(L, -1);
        lua_settop(L, 0);
        luaObj ret(F(std::move(param[0]), std::move(param[1]), std::move(param[2]), std::move(param[3])));
        ret.toLuaStack(L);
        return 1;
    }
    template <typename P1, typename P2, typename P3> static int lsFunc(lua_State *L){
        luaObj param[3];
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop, 3);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        lua_pushvalue(L, lua_upvalueindex(1));
        T(*F)(P1, P2, P3) = (T(*)(P1, P2, P3))lua_touserdata(L, -1);
        lua_settop(L, 0);
        luaObj ret(F(std::move(param[0]), std::move(param[1]), std::move(param[2])));
        ret.toLuaStack(L);
        return 1;
    }
    template <typename P1, typename P2> static int lsFunc(lua_State *L){
        luaObj param[2];
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop, 2);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        lua_pushvalue(L, lua_upvalueindex(1));
        T(*F)(P1, P2) = (T(*)(P1, P2))lua_touserdata(L, -1);
        lua_settop(L, 0);
        luaObj ret(F(std::move(param[0]), std::move(param[1])));
        ret.toLuaStack(L);
        return 1;
    }
    template <typename P1> static int lsFunc(lua_State *L){
        luaObj param;
        int stackTop = lua_gettop(L);
        if(stackTop) param.fromLuaStack(L, -stackTop + 0);

        lua_pushvalue(L, lua_upvalueindex(1));
        T(*F)(P1) = (T(*)(P1))lua_touserdata(L, -1);
        lua_settop(L, 0);
        luaObj ret(F(std::move(param)));
        ret.toLuaStack(L);
        return 1;
    }
    static int lsFunc(lua_State *L){
        int stackTop = lua_gettop(L);
        lua_pushvalue(L, lua_upvalueindex(1));
        T(*F)() = (T(*)())lua_touserdata(L, -1);
        lua_settop(L, 0);
        luaObj ret(F());
        ret.toLuaStack(L);
        return 1;
    }
};
template <> struct lsFunctionBinder <void> {
    template <typename P1, typename P2, typename P3, typename P4> static int lsFunc(lua_State *L){
        luaObj param[4];
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop, 4);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        lua_pushvalue(L, lua_upvalueindex(1));
        void(*F)(P1, P2, P3, P4) = (void(*)(P1, P2, P3, P4))lua_touserdata(L, -1);
        lua_settop(L, 0);
        F(std::move(param[0]), std::move(param[1]), std::move(param[2]), std::move(param[3]));
        return 0;
    }
    template <typename P1, typename P2, typename P3> static int lsFunc(lua_State *L){
        luaObj param[3];
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop, 3);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        lua_pushvalue(L, lua_upvalueindex(1));
        void(*F)(P1, P2, P3) = (void(*)(P1, P2, P3))lua_touserdata(L, -1);
        lua_settop(L, 0);
        F(std::move(param[0]), std::move(param[1]), std::move(param[2]));
        return 0;
    }
    template <typename P1, typename P2> static int lsFunc(lua_State *L){
        luaObj param[2];
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop, 2);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        lua_pushvalue(L, lua_upvalueindex(1));
        void(*F)(P1, P2) = (void(*)(P1, P2))lua_touserdata(L, -1);
        lua_settop(L, 0);
        F(std::move(param[0]), std::move(param[1]));
        return 0;
    }
    template <typename P1> static int lsFunc(lua_State *L){
        luaObj param;
        int stackTop = lua_gettop(L);
        if(stackTop) param.fromLuaStack(L, -stackTop + 0);

        lua_pushvalue(L, lua_upvalueindex(1));
        void(*F)(P1) = (void(*)(P1))lua_touserdata(L, -1);
        lua_settop(L, 0);
        F(std::move(param));
        return 0;
    }
    static int lsFunc(lua_State *L){
        int stackTop = lua_gettop(L);
        lua_pushvalue(L, lua_upvalueindex(1));
        void(*F)() = (void(*)())lua_touserdata(L, -1);
        lua_settop(L, 0);
        F();
        return 0;
    }
};

class lsClassBinder{
public:
    template <class C> static int stdMethod(lua_State *L){
        int stackTop = lua_gettop(L);
        luaObj pObj;

        pObj.fromLuaStack(L, -stackTop);
        C *p = (C *)(void *)pObj;

        lua_pushvalue(L, lua_upvalueindex(1));
        int (C::*F)(luaScript *) = *(int (C::**)(luaScript *))lua_touserdata(L, -1);
        lua_pushvalue(L, lua_upvalueindex(2));
        luaScript *tlua = (luaScript *)lua_touserdata(L, -1);

        lua_settop(L, stackTop);
        lua_remove(L, -stackTop);

        return (p->*F)(tlua);
    }

    template <class C> static int stdMethodAsGlobal(lua_State *L){
        int stackTop = lua_gettop(L);

        lua_pushvalue(L, lua_upvalueindex(1));
        luaScript *tlua = (luaScript *)lua_touserdata(L, -1);
        lua_pushvalue(L, lua_upvalueindex(2));
        C *p = (C *)lua_touserdata(L, -1);
        lua_pushvalue(L, lua_upvalueindex(3));
        int (C::*F)(luaScript *) = *(int (C::**)(luaScript *))lua_touserdata(L, -1);

        lua_settop(L, stackTop);
        return (p->*F)(tlua);
    }

    template <typename T> struct MethodBinder {
        template <class C> static int lsFunc(lua_State *L){
            int stackTop = lua_gettop(L);
            luaObj pObj;

            pObj.fromLuaStack(L, -stackTop);
            C *p = (C *)(void *)pObj;

            lua_pushvalue(L, lua_upvalueindex(1));
            T(C::*F)() = *(T(C::**)())lua_touserdata(L, -1);

            lua_settop(L, 0);
            luaObj ret((p->*F)());
            ret.toLuaStack(L);
            return 1;
        }
        template <class C, typename P1> static int lsFunc(lua_State *L){
            int stackTop = lua_gettop(L);
            luaObj pObj, param;

            pObj.fromLuaStack(L, -stackTop);
            param.fromLuaStack(L, -stackTop + 1);
            C *p = (C *)(void *)pObj;

            lua_pushvalue(L, lua_upvalueindex(1));
            T(C::*F)(P1) = *(T(C::**)(P1))lua_touserdata(L, -1);

            lua_settop(L, 0);
            luaObj ret((p->*F)(std::move(param)));
            ret.toLuaStack(L);
            return 1;
        }
        template <class C, typename P1, typename P2> static int lsFunc(lua_State *L){
            int i, stackTop = lua_gettop(L);
            int cnt = std::min(stackTop - 1, 2);
            luaObj pObj, param[2];

            pObj.fromLuaStack(L, -stackTop);
            for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i + 1);
            C *p = (C *)(void *)pObj;

            lua_pushvalue(L, lua_upvalueindex(1));
            T(C::*F)(P1, P2) = *(T(C::**)(P1, P2))lua_touserdata(L, -1);

            lua_settop(L, 0);
            luaObj ret((p->*F)(std::move(param[0]), std::move(param[1])));
            ret.toLuaStack(L);
            return 1;
        }
        template <class C, typename P1, typename P2, typename P3> static int lsFunc(lua_State *L){
            int i, stackTop = lua_gettop(L);
            int cnt = std::min(stackTop - 1, 3);
            luaObj pObj, param[3];

            pObj.fromLuaStack(L, -stackTop);
            for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i + 1);
            C *p = (C *)(void *)pObj;

            lua_pushvalue(L, lua_upvalueindex(1));
            T(C::*F)(P1, P2, P3) = *(T(C::**)(P1, P2, P3))lua_touserdata(L, -1);

            lua_settop(L, 0);
            luaObj ret((p->*F)(std::move(param[0]), std::move(param[1]), std::move(param[2])));
            ret.toLuaStack(L);
            return 1;
        }
        template <class C, typename P1, typename P2, typename P3, typename P4> static int lsFunc(lua_State *L){
            int i, stackTop = lua_gettop(L);
            int cnt = std::min(stackTop - 1, 4);
            luaObj pObj, param[4];

            pObj.fromLuaStack(L, -stackTop);
            for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i + 1);
            C *p = (C *)(void *)pObj;

            lua_pushvalue(L, lua_upvalueindex(1));
            T(C::*F)(P1, P2, P3, P4) = *(T(C::**)(P1, P2, P3, P4))lua_touserdata(L, -1);

            lua_settop(L, 0);
            luaObj ret((p->*F)(std::move(param[0]), std::move(param[1]), std::move(param[2]), std::move(param[3])));
            ret.toLuaStack(L);
            return 1;
        }
    };

    template <class C> static int destructor(lua_State *L){
        luaObj param;
        param.fromLuaStack(L, -1);

        C *p = (C *)(void *)param;

        delete p;
        return 0;
    }

    template <class C> static int constructor(lua_State *L){
        char bufMeta[256];

        strcpy(bufMeta, lua_tostring(L, lua_upvalueindex(1)));
        C **ppObj = (C **)lua_newuserdata(L, sizeof(C *));
        *ppObj = new C();

        int  stackTop = lua_gettop(L);
        luaL_getmetatable(L, bufMeta);
        if(lua_istable(L, -1)){
            lua_setmetatable(L, -2);
        }
        lua_settop(L, stackTop);
        return 1;
    }
    template <class C, typename P1> static int constructor(lua_State *L){
        char bufMeta[256];
        int  stackTop = lua_gettop(L);
        strcpy(bufMeta, lua_tostring(L, lua_upvalueindex(1)));

        luaObj param;
        param.fromLuaStack(L, -stackTop);

        C **ppObj = (C **)lua_newuserdata(L, sizeof(C *));
        *ppObj = new C(std::move(param));

        stackTop = lua_gettop(L);
        luaL_getmetatable(L, bufMeta);
        if(lua_istable(L, -1)){
            lua_setmetatable(L, -2);
        }
        lua_settop(L, stackTop);
        return 1;
    }
    template <class C, typename P1, typename P2> static int constructor(lua_State *L){
        char bufMeta[256];
        int  i, stackTop = lua_gettop(L);
        int  cnt = std::min(stackTop, 2);
        strcpy(bufMeta, lua_tostring(L, lua_upvalueindex(1)));

        luaObj param[2];
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        C **ppObj = (C **)lua_newuserdata(L, sizeof(C *));
        *ppObj = new C(std::move(param[0]), std::move(param[1]));

        stackTop = lua_gettop(L);
        luaL_getmetatable(L, bufMeta);
        if(lua_istable(L, -1)){
            lua_setmetatable(L, -2);
        }
        lua_settop(L, stackTop);
        return 1;
    }
    template <class C, typename P1, typename P2, typename P3> static int constructor(lua_State *L){
        char bufMeta[256];
        int  i, stackTop = lua_gettop(L);
        int  cnt = std::min(stackTop, 3);
        strcpy(bufMeta, lua_tostring(L, lua_upvalueindex(1)));

        luaObj param[3];
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        C **ppObj = (C **)lua_newuserdata(L, sizeof(C *));
        *ppObj = new C(std::move(param[0]), std::move(param[1]), std::move(param[2]));

        stackTop = lua_gettop(L);
        luaL_getmetatable(L, bufMeta);
        if(lua_istable(L, -1)){
            lua_setmetatable(L, -2);
        }
        lua_settop(L, stackTop);
        return 1;
    }
    template <class C, typename P1, typename P2, typename P3, typename P4> static int constructor(lua_State *L){
        char bufMeta[256];
        int  i, stackTop = lua_gettop(L);
        int  cnt = std::min(stackTop, 4);
        strcpy(bufMeta, lua_tostring(L, lua_upvalueindex(1)));

        luaObj param[4];
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i);

        C **ppObj = (C **)lua_newuserdata(L, sizeof(C *));
        *ppObj = new C(std::move(param[0]), std::move(param[1]), std::move(param[2]), std::move(param[3]));

        stackTop = lua_gettop(L);
        luaL_getmetatable(L, bufMeta);
        if(lua_istable(L, -1)){
            lua_setmetatable(L, -2);
        }
        lua_settop(L, stackTop);
        return 1;
    }
};

template <> struct lsClassBinder::MethodBinder <void> {
    template <class C> static int lsFunc(lua_State *L){
        int stackTop = lua_gettop(L);
        luaObj pObj;

        pObj.fromLuaStack(L, -stackTop);
        C *p = (C *)(void *)pObj;

        lua_pushvalue(L, lua_upvalueindex(1));
        void (C::*F)() = *(void (C::**)())lua_touserdata(L, -1);

        lua_settop(L, 0);
        (p->*F)();
        return 0;
    }
    template <class C, typename P1> static int lsFunc(lua_State *L){
        int stackTop = lua_gettop(L);
        luaObj pObj, param;

        pObj.fromLuaStack(L, -stackTop);
        param.fromLuaStack(L, -stackTop + 1);
        C *p = (C *)(void *)pObj;

        lua_pushvalue(L, lua_upvalueindex(1));
        void (C::*F)(P1) = *(void (C::**)(P1))lua_touserdata(L, -1);

        lua_settop(L, 0);
        (p->*F)(std::move(param));
        return 0;
    }
    template <class C, typename P1, typename P2> static int lsFunc(lua_State *L){
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop - 1, 2);
        luaObj pObj, param[2];

        pObj.fromLuaStack(L, -stackTop);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i + 1);
        C *p = (C *)(void *)pObj;

        lua_pushvalue(L, lua_upvalueindex(1));
        void (C::*F)(P1, P2) = *(void (C::**)(P1, P2))lua_touserdata(L, -1);

        lua_settop(L, 0);
        (p->*F)(std::move(param[0]), std::move(param[1]));
        return 0;
    }
    template <class C, typename P1, typename P2, typename P3> static int lsFunc(lua_State *L){
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop - 1, 3);
        luaObj pObj, param[3];

        pObj.fromLuaStack(L, -stackTop);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i + 1);
        C *p = (C *)(void *)pObj;

        lua_pushvalue(L, lua_upvalueindex(1));
        void (C::*F)(P1, P2, P3) = *(void (C::**)(P1, P2, P3))lua_touserdata(L, -1);

        lua_settop(L, 0);
        (p->*F)(std::move(param[0]), std::move(param[1]), std::move(param[2]));
        return 0;
    }
    template <class C, typename P1, typename P2, typename P3, typename P4> static int lsFunc(lua_State *L){
        int i, stackTop = lua_gettop(L);
        int cnt = std::min(stackTop - 1, 4);
        luaObj pObj, param[4];

        pObj.fromLuaStack(L, -stackTop);
        for(i = 0; i<cnt; i++) param[i].fromLuaStack(L, -stackTop + i + 1);
        C *p = (C *)(void *)pObj;

        lua_pushvalue(L, lua_upvalueindex(1));
        void (C::*F)(P1, P2, P3, P4) = *(void (C::**)(P1, P2, P3, P4))lua_touserdata(L, -1);

        lua_settop(L, 0);
        (p->*F)(std::move(param[0]), std::move(param[1]), std::move(param[2]), std::move(param[3]));
        return 0;
    }
};

enum LUAMODE {
    LUAMODE_CONFIG = 0,
    LUAMODE_SCRIPT = 1,
};

struct luaScript {                                                           //--- lua script object
    LUAMODE      _mode;
    char         _lastErrorMessage[256];
    unsigned int _userdataSize;
    void         *_userdata;
    lua_State    *_lua;

    luaScript();
    ~luaScript();

    bool    execLuaChunk(const char *icode, unsigned int isize, LUAMODE mode = LUAMODE_CONFIG);

    bool    reset();                                                         // clear VM

    void    getGlobalVar(const luaObj &key, luaObj &ovalue);
    luaObj  getGlobalVar(const luaObj &key) const;
    void    setGlobalVar(const luaObj &key, const luaObj &ivalue) const;

    void    regStdFunction(const char *funcName, int(*func)(luaScript *));

    int     callLuaFunc(const char *funcName);
    int     callLuaFunc(const char *funcName, const luaObj &p0);
    int     callLuaFunc(const char *funcName, const luaObj &p0, const luaObj &p1);
    int     callLuaFunc(const char *funcName, const luaObj &p0, const luaObj &p1, const luaObj &p2);
    int     callLuaFunc(const char *funcName, const luaObj &p0, const luaObj &p1, const luaObj &p2, const luaObj &p3);

    int     stackCount();
    luaObj  fromStack(int index) const;
    void    pushValue(const luaObj &val);

    template <class C> bool pushObject(const char *meta, C *obj);

    void    setUserData(void *idata, int size);
    void    getUserData(void *odata);
    int     getUserDataSize();

    const char *getLastError();

    bool    regMeta(const char *meta);

    template <class C> bool regInstanceObject(const char *instName, const char *meta, C *obj);
    template <class C> bool regStdMethod(const char *meta, const char *mName, int (C::*pf)(luaScript *));

    template <class C> void regClass(const char *meta, bool regDestructor = true);
    template <class C, typename P1> void regClass(const char *meta, bool regDestructor = true);
    template <class C, typename P1, typename P2> void regClass(const char *meta, bool regDestructor = true);
    template <class C, typename P1, typename P2, typename P3> void regClass(const char *meta, bool regDestructor = true);
    template <class C, typename P1, typename P2, typename P3, typename P4> void regClass(const char *meta, bool regDestructor = true);

    template <typename T> void regFunction(const char *funcName, T(*f)());
    template <typename T, typename P1> void regFunction(const char *funcName, T(*f)(P1));
    template <typename T, typename P1, typename P2> void regFunction(const char *funcName, T(*f)(P1, P2));
    template <typename T, typename P1, typename P2, typename P3> void regFunction(const char *funcName, T(*f)(P1, P2, P3));
    template <typename T, typename P1, typename P2, typename P3, typename P4> void regFunction(const char *funcName, T(*f)(P1, P2, P3, P4));

    template <typename T, class C> bool regMethod(const char *meta, const char *mName, T(C::*pf)());
    template <typename T, class C, typename P1> bool regMethod(const char *meta, const char *mName, T(C::*pf)(P1));
    template <typename T, class C, typename P1, typename P2> bool regMethod(const char *meta, const char *mName, T(C::*pf)(P1, P2));
    template <typename T, class C, typename P1, typename P2, typename P3> bool regMethod(const char *meta, const char *mName, T(C::*pf)(P1, P2, P3));
    template <typename T, class C, typename P1, typename P2, typename P3, typename P4> bool regMethod(const char *meta, const char *mName, T(C::*pf)(P1, P2, P3, P4));
};




