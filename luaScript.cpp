
#include "lua-5.1.4/all.c"

namespace std {
    std::size_t hash<luaObj>::operator()(const luaObj& t) const {
        if(t._type == LUATYPE_STRING) {
            size_t h = hash<std::string>()((const char *)(t._str.sz < LUAOBJ_STRMAX ? t._str.buf : t._str.farptr));
            return h;
        }
        else if(t._type == LUATYPE_NUMBER) return hash<double>()(t._std.num);
        else if(t._type == LUATYPE_BOOL) return hash<bool>()(t._std.boolean);
        else return hash<const void *>()(&t);
    }
}

luaObj _luaObj_empty;

char parseLuaObj2Bin(luaObj *inobj, unsigned char *oBin, int *ioBinCount){
  char fieldCount = 0;  
  luaObj::hashMap::iterator currentPair = inobj->_table->begin();

  while(currentPair != inobj->_table->end()){    
    const luaObj *tname = &currentPair->first;
    luaObj  *tvalue = currentPair->second;
    LUATYPE indexType = tname->_type;

    if(indexType == LUATYPE_STRING){
      unsigned char *offWrite = oBin + *ioBinCount;
      unsigned char strSz = tname->_str.sz < 255 ? (unsigned char)tname->_str.sz : 254;
      const char *str = tname->_str.sz < LUAOBJ_STRMAX ? tname->_str.buf : tname->_str.farptr; 

      *(char *)(offWrite+0) = 0x4;                       //indexType
      *(unsigned char *)(offWrite+1) = strSz;

      memcpy(offWrite+2, str, strSz);                    //indexValue

      *ioBinCount += (strSz+2);      
    }
    if(indexType == LUATYPE_NUMBER){
      float tnum = (float)tname->_std.num;
      int   tnumi = (int)tnum;
      unsigned char *offWrite = oBin + *ioBinCount;    

      if(fabs(tnum - (float)tnumi) < LUAOBJ_EPSILON){
        *(char *)(offWrite+0) = 0x1;
        *(int *)(offWrite+1) = tnumi;
      }
      else{
        *(char *)(offWrite+0) = 0x2;
        *(float *)(offWrite+1) = tnum;      
      }      
      *ioBinCount += 5;
    }
    if(indexType == LUATYPE_BOOL){
      unsigned char bVal = tname->_std.boolean ? 1 : 0;
      unsigned char *offWrite = oBin + *ioBinCount;
      *(unsigned char *)(offWrite+0) = 0x3;
      *(unsigned char *)(offWrite+1) = bVal;
      *ioBinCount += 2;
    }

    fieldCount++;
    LUATYPE valueType = tvalue->_type;

    if(valueType == LUATYPE_TABLE){
      unsigned char *offWrite = oBin + *ioBinCount;
      *ioBinCount += 2;
      *(char *)(offWrite+0) = 0x0;                                      //this is table
      *(char *)(offWrite+1) = parseLuaObj2Bin(currentPair->second, oBin, ioBinCount);  
    }
    else{
      if(valueType == LUATYPE_NUMBER){
        float tnum = (float)tvalue->_std.num;
        int   tnumi = (int)tnum;
        unsigned char *offWrite = oBin + *ioBinCount;      

        if(fabs(tnum - (float)tnumi) < LUAOBJ_EPSILON){
          *(char *)(offWrite+0) = 0x1;
          *(int *)(offWrite+1) = tnumi;
        }
        else{
          *(char *)(offWrite+0) = 0x2;
          *(float *)(offWrite+1) = tnum;      
        }      
        *ioBinCount += 5;
      }
      if(valueType == LUA_TSTRING){
        unsigned char *offWrite = oBin + *ioBinCount;
        unsigned char strSz = tvalue->_str.sz < 255 ? (unsigned char)tvalue->_str.sz : 254;
        const char *str = tvalue->_str.sz < LUAOBJ_STRMAX ? tvalue->_str.buf : tvalue->_str.farptr;              

        *(char *)(offWrite+0) = 0x4;                            //indexType
        *(unsigned char *)(offWrite+1) = strSz;

        memcpy(offWrite+2, str, strSz);                       //indexValue               
        *ioBinCount += (strSz+2);      
      }
      if(valueType == LUA_TBOOLEAN){
        unsigned char bVal = tvalue->_std.boolean ? 1 : 0;
        unsigned char *offWrite = oBin+*ioBinCount;
        *(unsigned char *)(offWrite+0) = 0x3;
        *(unsigned char *)(offWrite+1) = bVal;
        *ioBinCount += 2;
      }
    }       
    currentPair++;
  }                          
  return fieldCount;
}

void parseLuaObjFromBin(luaObj *objout, int fieldMax, unsigned char **curBin, unsigned char *iBinMax){
  int curFields = 0;  

  while(*curBin < iBinMax && curFields < fieldMax){
    luaObj varName;
    char indexType = *(char *)(*curBin+0);
    if(indexType == 0x1){  //int
      int ival = *(int *)(*curBin+1);
      varName = ival;
      *curBin += 5;
    }
    if(indexType == 0x2){  //float
      float ival = *(float *)(*curBin+1);
      varName = ival;
      *curBin += 5;
    }
    if(indexType == 0x3){  //bool
      unsigned char ival = *(unsigned char *)(*curBin+1);
      varName = ival ? true : false;
      *curBin += 2;
    }
    if(indexType == 0x4){  //string
      char tbuf[256] = {0};
      unsigned char strsz = *(unsigned char *)(*curBin+1);
      const char *ival = (char *)(*curBin+2);      
      memcpy(tbuf, ival, strsz);
      varName = tbuf;
      *curBin += (int)strsz+2;
    }

    char valueType = *(char *)(*curBin+0);

    if(valueType == 0x0){ // is table
      char tfields = *(char *)(*curBin+1);
      *curBin += 2;   
      luaObj varValue;

      parseLuaObjFromBin(&varValue, (int)tfields, curBin, iBinMax);
      objout->add(varName, varValue);
    }
    else{      
      if(valueType == 0x1){  //int
        int vval = *(int *)(*curBin+1);
        luaObj varValue = vval;
        objout->add(varName, varValue);
        *curBin += 5;
      }
      if(valueType == 0x2){  //float
        float vval = *(float *)(*curBin+1);
        luaObj varValue = vval;
        objout->add(varName, varValue);        
        *curBin += 5;
      }
      if(valueType == 0x3){  //bool
        char vval = *(char *)(*curBin+1);
        luaObj varValue = vval ? true : false;
        objout->add(varName, varValue);
        *curBin += 2;
      }
      if(valueType == 0x4){  //string
        char tbuf[256] = {0};
        unsigned char strsz = *(unsigned char *)(*curBin+1);
        const char *vval = (char *)(*curBin+2);
        memcpy(tbuf, vval, strsz);

        luaObj varValue = tbuf;
        objout->add(varName, varValue);
        *curBin += strsz+2;
      }
    } 

    curFields++;
  }
}

//---------------------------------------------------------------------------

luaObj::luaObj(const char *str) : _type(LUATYPE_STRING), _table(nullptr){
  _str.sz = strlen(str) + 1;
  if(_str.sz < LUAOBJ_STRMAX){
    memcpy(_str.buf, str, _str.sz);
  }
  else{
    _str.farptr = new char [_str.sz];
    memcpy(_str.farptr, str, _str.sz);
  }
}

luaObj::luaObj(const luaObj &copy) : _table(0){
  _type = copy._type;
  if(_type == LUATYPE_TABLE){
    _table = new hashMap ();

    hashMap ::const_iterator index = copy._table->begin();
    while(index != copy._table->end()){
      _table->emplace(std::pair <luaObj, luaObj *> (index->first, new luaObj(*index->second)));
      ++index;
    }
  }
  else if(_type == LUATYPE_STRING){
    _str.sz = copy._str.sz;

    if(_str.sz < LUAOBJ_STRMAX){
      memcpy(_str.buf, copy._str.buf, _str.sz);
    }
    else{
      _str.farptr = new char [_str.sz];
      memcpy(_str.farptr, copy._str.farptr, _str.sz);
    }
  }
  else _std = copy._std;  
}  

luaObj::luaObj(luaObj &&from) : _type(from.type()), _table(from._table){
  from._table = nullptr;
  from._type  = LUATYPE_NULL;

  if(_type == LUATYPE_STRING){
    _str.sz = from._str.sz;
    if(_str.sz < LUAOBJ_STRMAX) memcpy(_str.buf, from._str.buf, _str.sz);
    else{
      _str.farptr = from._str.farptr;
      from._str.farptr = nullptr;
    }
  }
  else if(_type != LUATYPE_TABLE){
    _std = std::move(from._std);
  }
}

luaObj::~luaObj(){
  if(_table){
    clear();
    delete _table;
  }
  if(_str.farptr) delete _str.farptr;                                     // not []       
}

luaObj::operator char () const{
  int reu = 0;
  if(_type == LUATYPE_NUMBER) reu = (int)_std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atoi(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1 : 0;
  }
  return (char)reu;
}

luaObj::operator unsigned char () const{
  int reu = 0;
  if(_type == LUATYPE_NUMBER) reu = (int)_std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atoi(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1 : 0;
  }
  return (unsigned char)reu;
}

luaObj::operator short () const{
  int reu = 0;
  if(_type == LUATYPE_NUMBER) reu = (int)_std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atoi(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1 : 0;
  }
  return (short)reu;
}

luaObj::operator unsigned short () const{
  int reu = 0;
  if(_type == LUATYPE_NUMBER) reu = (int)_std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atoi(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1 : 0;
  }
  return (unsigned short)reu;
}

luaObj::operator int () const{
  int reu = 0;
  if(_type == LUATYPE_NUMBER) reu = (int)_std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atoi(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1 : 0;
  }
  return (int)reu;
}

luaObj::operator unsigned int () const{
  int reu = 0;
  if(_type == LUATYPE_NUMBER) reu = (int)_std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atoi(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1 : 0;
  }
  return (unsigned int)reu;
}

luaObj::operator float () const{
  double reu = 0;
  if(_type == LUATYPE_NUMBER) reu = _std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atof(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1.0 : 0.0;
  }
  return (float)reu;
}

luaObj::operator double () const{
  double reu = 0;
  if(_type == LUATYPE_NUMBER) reu = _std.num;
  else{
    if(_type == LUATYPE_STRING) reu = atof(_str.buf);
    else if(_type == LUATYPE_BOOL) reu = _std.boolean ? 1.0 : 0.0;
  }
  return reu;
}

luaObj::operator const char * () const{
  if(_type == LUATYPE_STRING){
    if(_str.sz < LUAOBJ_STRMAX) return _str.buf;
    else return _str.farptr;
  }
  else if(_type == LUATYPE_NUMBER){
    int dnum = (int)_std.num;
    if(fabs(_std.num - (float)dnum) > LUAOBJ_EPSILON) _gcvt_s(_str.buf, _std.num, 10);
    else _itoa_s(dnum, _str.buf, 10);
    return _str.buf;
  }
  else{
    _str.buf[0] = 0;
    return _str.buf;
  }
}

luaObj::operator bool () const{
  if(_type == LUATYPE_BOOL) return _std.boolean;
  else if(_type == LUATYPE_NUMBER) return fabs(_std.num) > LUAOBJ_EPSILON ? true : false;
  return false;
}

luaObj::operator void * () const{
  if(_type == LUATYPE_USERDATA) return _std.ptr;
  else return 0;
}

luaObj &luaObj::operator = (const luaObj &copy){
  if(&copy == this) return *this;
  if(copy._type == LUATYPE_TABLE){
    if(!_table){
      _table = new hashMap ();
    }
    else clear();
    
    hashMap ::const_iterator index = copy._table->begin();
    while(index != copy._table->end()){
      _table->emplace(std::pair <luaObj, luaObj *> (index->first, new luaObj(*index->second)));
      ++index;
    }
  }
  else{
    if(copy._type == LUATYPE_STRING){
      _str.sz = copy._str.sz;
      if(_str.sz < LUAOBJ_STRMAX){
        memcpy(_str.buf, copy._str.buf, _str.sz);
      }
      else{
        _str.farptr = new char [_str.sz];
        memcpy(_str.farptr, copy._str.farptr, _str.sz);
      }
    }
    else{
      _std = copy._std;
    }
  }
  _type = copy._type;
  return *this;
}

luaObj &luaObj::operator = (luaObj &&rVal){
  if(&rVal == this) return *this;

  _type = rVal._type;
  rVal._type = LUATYPE_NULL;
  
  if(_type == LUATYPE_TABLE){
    _table = rVal._table;
    rVal._table = nullptr;
  }
  else if(_type == LUATYPE_STRING){
    _str.sz = rVal._str.sz;
    if(_str.sz < LUAOBJ_STRMAX) memcpy(_str.buf, rVal._str.buf, _str.sz);
    else{
      _str.farptr = rVal._str.farptr;
      rVal._str.farptr = nullptr;
    }
  }
  else{
    _std = std::move(rVal._std);
  }
  return *this;
}

bool luaObj::operator == (const luaObj &rVal) const{
  if(_type != rVal._type){
    return false;
  }
  else if(_type == LUATYPE_STRING){
    const char *str1 = _str.sz < LUAOBJ_STRMAX ? _str.buf : _str.farptr;
    const char *str2 = rVal._str.sz < LUAOBJ_STRMAX ? rVal._str.buf : rVal._str.farptr;
    return !strcmp(str1, str2);
  }
  else if(_type == LUATYPE_NUMBER){
    return fabs(_std.num - rVal._std.num) < LUAOBJ_EPSILON;
  }
  else if(_type == LUATYPE_BOOL){
    return _std.boolean == rVal._std.boolean;
  }
  else return false;
}

bool luaObj::operator < (const luaObj &rVal) const{
  if(_type == rVal._type){
    if(_type == LUATYPE_STRING){
      const char *str1 = _str.sz < LUAOBJ_STRMAX ? _str.buf : _str.farptr;
      const char *str2 = rVal._str.sz < LUAOBJ_STRMAX ? rVal._str.buf : rVal._str.farptr;
      return strcmp(str1, str2) < 0;
    }
    else if(_type == LUATYPE_NUMBER) return _std.num < rVal._std.num;
    else if(_type == LUATYPE_BOOL) return !_std.boolean && rVal._std.boolean; 
    else return this < &rVal;  
  }
  else return _type < rVal._type;  
}

luaObj &luaObj::add(const luaObj &key, const luaObj &value){
  if(_type != LUATYPE_TABLE){
    _table = new hashMap ();
    _type = LUATYPE_TABLE;
  }
  else{
    hashMap ::iterator index = _table->find(key);  
    if(index != _table->end()){
      *index->second = value;
      return *index->second;
    }
  }
  return *_table->emplace(std::pair <luaObj, luaObj *> (key, new luaObj(value))).first->second;
}

luaObj &luaObj::add(luaObj &&key, luaObj &&value){
  if(_type != LUATYPE_TABLE){
    _table = new hashMap();
    _type  = LUATYPE_TABLE;
  }
  else{
    hashMap ::iterator index = _table->find(key);  
    if(index != _table->end()){
      *index->second = std::move(value);
      return *index->second;
    }
  }
  return *_table->emplace(std::pair <luaObj, luaObj *> (std::move(key), new luaObj(std::move(value)))).first->second;
}

luaObj &luaObj::add(const luaObj &key, luaObj &&value){
  if(_type != LUATYPE_TABLE){
    _table = new hashMap();
    _type  = LUATYPE_TABLE;
  }
  else{
    hashMap ::iterator index = _table->find(key);  
    if(index != _table->end()){
      *index->second = std::move(value);
      return *index->second;
    }
  }
  return *_table->emplace(std::pair <luaObj, luaObj *> (key, new luaObj(std::move(value)))).first->second;
}

luaObj &luaObj::add(luaObj &&key, const luaObj &value){
  if(_type != LUATYPE_TABLE){
    _table = new hashMap();
    _type  = LUATYPE_TABLE;
  }
  else{
    hashMap ::iterator index = _table->find(key);  
    if(index != _table->end()){
      *index->second = value;
      return *index->second;
    }
  }
  return *_table->emplace(std::pair <luaObj, luaObj *> (std::move(key), new luaObj(value))).first->second;
}

luaObj &luaObj::push(const luaObj &key, const luaObj &value){                     
  add(key, value);
  return *this;
}

luaObj &luaObj::push(luaObj &&key, luaObj &&value){
  add(key, value);
  return *this;
}

luaObj &luaObj::push(const luaObj &key, luaObj &&value){
  add(key, value);
  return *this;
}

luaObj &luaObj::push(luaObj &&key, const luaObj &value){
  add(key, value);
  return *this;
}

luaObj &luaObj::get(const luaObj &key) const{                              
  if(_type == LUATYPE_TABLE){
    hashMap::iterator index = _table->find(key);
    if(index != _table->end()){
      return (*index->second);
    }
  }
  _luaObj_empty._type = LUATYPE_NULL;
  return _luaObj_empty;
}

luaObj *luaObj::getptr(const luaObj &key){                                 
  if(_type == LUATYPE_TABLE){
    hashMap::iterator index = _table->find(key);
    if(index != _table->end()){
      return index->second;
    }
  }  
  return nullptr;
}

unsigned int luaObj::count() const{                                        
  if(_type == LUATYPE_TABLE) return _table->size();
  return 0;  
}

void luaObj::clear(){                                                      
  if(_table){
    hashMap::iterator index = _table->begin();
    while(index != _table->end()){
      delete index->second;
      index++;
    }         
    _table->clear();
  }
}

bool luaObj::remove(const luaObj &key){
  if(_type == LUATYPE_TABLE){
    hashMap::iterator index = _table->find(key);
    if(index != _table->end()){
      delete index->second;
      _table->erase(index);
      return true;
    }
  }
  return false;
}

void luaObj::toLuaStack(lua_State *L) const{
  if(_type == LUATYPE_NUMBER){
    lua_pushnumber(L, (double)_std.num);    
  }
  else if(_type == LUATYPE_STRING){
    lua_pushstring(L, _str.sz < LUAOBJ_STRMAX ? _str.buf : _str.farptr);  
  }
  else if(_type == LUATYPE_USERDATA){
    void **p = (void **)lua_newuserdata(L, sizeof(void *));
    *p = _std.ptr;

    int stackTop = lua_gettop(L);
    luaL_getmetatable(L, getMeta());

    if(lua_istable(L, -1)){ 
      lua_setmetatable(L, -2);      
    }
    else{
      lua_settop(L, stackTop);
    }
  }
  else if(_type == LUATYPE_TABLE){
    lua_newtable(L);

    hashMap ::const_iterator index = _table->begin();
    while(index != _table->end()){

      index->first.toLuaStack(L);
      index->second->toLuaStack(L);
      lua_rawset(L, -3);

      index++;
    } 
  }
  else if(_type == LUATYPE_BOOL){
    lua_pushboolean(L, _std.boolean ? 1 : 0);
  }
  else lua_pushnil(L);
}

void luaObj::fromLuaStack(lua_State *L, int idx){
  LUATYPE ttype = (LUATYPE)lua_type(L, idx);

  if(ttype == LUATYPE_TABLE){
    if(!_table){
      _table = new hashMap ();
    }
    else{
      clear();
    }

    int savedTop = lua_gettop(L);
    lua_pushvalue(L, idx);
    int ttop = lua_gettop(L);
    lua_pushnil(L);

    while(lua_next(L, ttop)){
      // -2 name
      // -1 value

      _table->emplace(std::make_pair(std::move(luaObj(L, -2)), std::move(new luaObj(L, -1))));
      lua_pop(L, 1);
    }    

    lua_settop(L, savedTop);
    _type = LUATYPE_TABLE;
  }
  else if(ttype == LUATYPE_STRING){
    const char *tstr = lua_tostring(L, idx);
    _str.sz = strlen(tstr) + 1;

    if(_str.sz < LUAOBJ_STRMAX){
      memcpy(_str.buf, tstr,_str.sz);
    }
    else{
      if(_str.farptr){
        delete _str.farptr;
      }
      _str.farptr = new char [_str.sz];
      memcpy(_str.farptr, tstr, _str.sz);
    }
    _type = LUATYPE_STRING;
  }
  else{
    if(ttype == LUATYPE_NUMBER){
      _std.num = lua_tonumber(L, idx);
      _type = LUATYPE_NUMBER;
    }
    else if(ttype == LUATYPE_BOOL){
      _std.boolean = lua_toboolean(L, idx) == 1 ? true : false;
      _type = LUATYPE_BOOL;
    }
    else if(ttype == LUATYPE_USERDATA){
      _std.ptr = *(void **)lua_touserdata(L, idx);
      _type = LUATYPE_USERDATA;
    }
    else if(ttype == LUA_TLIGHTUSERDATA){
      _std.ptr = lua_touserdata(L, idx);
      _type = LUATYPE_USERDATA;
    }
    else if(ttype == LUATYPE_CFUNC){
      _std.ptr = (void *)lua_tocfunction(L, idx);
      _type = LUATYPE_USERDATA;
    }
    else{
      _type = LUATYPE_NULL;
    }
  }
}

int luaObj::tableExport(unsigned char *oBin){
  int binCount = 0;         
  if(_type == LUATYPE_TABLE){
    parseLuaObj2Bin(this, oBin, &binCount);
  }
  return binCount;
}

void luaObj::tableImport(unsigned char *iBin, int iBinSize){
  if(_type != LUATYPE_TABLE){
    _table = new hashMap ();
    _type = LUATYPE_TABLE;
  }
  else clear();

  unsigned char *ioBin = iBin;
  parseLuaObjFromBin(this, 999, &ioBin, iBin + iBinSize);
}

char  _luaObjTableImportBuffer[65536];

void luaObj::tableImport(const byteform & target){
    while(true){
        unsigned char keyType = target.readByte();
        luaObj key, value;

        if(keyType == 0x01){
            target.readString(_luaObjTableImportBuffer);
            key = _luaObjTableImportBuffer;
        }
        else if(keyType == 0x02){
            key = target.readFloat();
        }
        else if(keyType == 0x03){
            key = target.readByte() == 0 ? false : true;
        }
        else break;

        unsigned char valueType = target.readByte();

        if(valueType == 0x0){
            value.tableImport(target);
        }
        else if(valueType == 0x01){
            target.readString(_luaObjTableImportBuffer);
            value = _luaObjTableImportBuffer;
        }
        else if(valueType == 0x02){
            value = target.readFloat();
        }
        else if(valueType == 0x03){
            value = target.readByte() == 0 ? false : true;
        }

        add(std::move(key), std::move(value));
    }
}

//---------------------------------------------------------------------------

int lsNativeFunction(lua_State *lS){
  luaScript *_lua = (luaScript *)lua_touserdata(lS, lua_upvalueindex(1));  
  int (*func)(luaScript *) = (int (*)(luaScript *))lua_touserdata(lS, lua_upvalueindex(2));

  return func(_lua);
} 

template <class C, typename F> void lsRegClass(lua_State *_lua, const char *meta, F bindFunc, bool regDestructor = true){
  int stackTop = lua_gettop(_lua);
  luaL_newmetatable(_lua, meta);
  lua_pushstring(_lua, "__index");  
  lua_newtable(_lua);         
  lua_rawset(_lua, -3);             

  if(regDestructor){
    lua_pushstring(_lua, "__gc");
    lua_pushcclosure(_lua, (int (*)(lua_State *))(lsClassBinder::destructor <C>), 0);
    lua_rawset(_lua, -3);
  }

  lua_pushstring(_lua, meta);
  lua_pushcclosure(_lua, func, 1);
  lua_setglobal(_lua, meta);      
  lua_settop(_lua, stackTop);
}

template <typename T, typename F> void lsRegFunction(lua_State *_lua, const char *funcName, void *fptr, F bindFunc){
  lua_pushlightuserdata(_lua, fptr);
  int (*tf)(lua_State *) = bindFunc;
  lua_pushcclosure(_lua, tf, 1);
  lua_setglobal(_lua, funcName);
}

template <typename F, typename FB> bool lsRegMethod(const char *meta, const char *mName, F fptr, FB bindFunc){
  int stackTop = lua_gettop(_lua); 
  luaL_getmetatable(_lua, meta);

  if(!lua_istable(_lua, -1)){
    sprintf_s(_lastErrorMessage, "luaScript error: can't find metatable %s", meta);      
    return false;
  }                       
  lua_pushstring(_lua, "__index");
  lua_rawget(_lua, -2);
  lua_pushstring(_lua, mName);

  *(F *)lua_newuserdata(_lua, sizeof(decltype(fptr))) = fptr;
  lua_pushcclosure(_lua, bindFunc, 1);
  lua_rawset(_lua, -3);       
  lua_settop(_lua, stackTop);    
  return true;
} 

//---------------------------------------------------------------------------

luaScript::luaScript(){
  _mode = LUAMODE_CONFIG;
  _lua = lua_open();
  if(!_lua){
    strcpy_s(_lastErrorMessage, "can't create lua VM");
    return;
  }

  _userdata = 0;
  _userdataSize = 0;
  _lastErrorMessage[0] = 0;
}

luaScript::~luaScript(){
  if(_userdataSize){
    free(_userdata);
  }
  if(_lua) lua_close(_lua);
}

bool luaScript::execLuaChunk(const char *icode, unsigned int isize, LUAMODE mode){
  if(mode == LUAMODE_SCRIPT) luaL_openlibs(_lua);

  int stack = lua_gettop(_lua);
  int result = luaL_loadbuffer(_lua, icode, isize, "exec");
  stack = lua_gettop(_lua);

  if(!result) result |= lua_pcall(_lua, 0, LUA_MULTRET, 0);
  if(result){
    strcpy_s(_lastErrorMessage, lua_tostring(_lua, -1));
    lua_close(_lua);
    _lua = 0;
    return false;
  }

  lua_settop(_lua, stack);
  return true;
}

bool luaScript::reset(){                                                       
  if(_lua) lua_close(_lua);
  _lua = lua_open();

  if(!_lua){
    strcpy_s(_lastErrorMessage, "can't create lua VM");
    return false;
  }                            

  _lastErrorMessage[0] = 0;
  return true;
}

void luaScript::getGlobalVar(const luaObj &key, luaObj &ovalue){
  int top = lua_gettop(_lua);
  key.toLuaStack(_lua);
  lua_gettable(_lua, LUA_GLOBALSINDEX);
  ovalue.fromLuaStack(_lua, -1);
  lua_settop(_lua, top);
}

luaObj luaScript::getGlobalVar(const luaObj &key) const{
  int top = lua_gettop(_lua);
  luaObj result;
  key.toLuaStack(_lua);
  lua_gettable(_lua, LUA_GLOBALSINDEX);
  result.fromLuaStack(_lua, -1);
  lua_settop(_lua, top);
  return result;
}

void luaScript::setGlobalVar(const luaObj &key, const luaObj &ivalue) const{
  key.toLuaStack(_lua);
  ivalue.toLuaStack(_lua);
  lua_settable(_lua, LUA_GLOBALSINDEX);
}

void luaScript::regStdFunction(const char *funcName, int (*func)(luaScript *)){        
  lua_pushlightuserdata(_lua, this);
  lua_pushlightuserdata(_lua, func);
  lua_pushcclosure(_lua, lsNativeFunction, 2);
  lua_setglobal(_lua, funcName);
} 

int luaScript::callLuaFunc(const char *funcName){                 
  lua_settop(_lua, 0);  
  lua_getglobal(_lua, funcName);
  if(lua_isnoneornil(_lua, -1)) return 0;

  if(lua_pcall(_lua, 0, -1, 0)){
    strcpy_s(_lastErrorMessage, lua_tostring(_lua, -1));
    return -1;    
  }
  else _lastErrorMessage[0] = 0;
  return 1;
}    

int luaScript::callLuaFunc(const char *funcName, const luaObj &p0){
  lua_settop(_lua, 0);  
  lua_getglobal(_lua, funcName);
  if(lua_isnoneornil(_lua, -1)) return 0;

  p0.toLuaStack(_lua);

  if(lua_pcall(_lua, 1, -1, 0)){
    strcpy_s(_lastErrorMessage, lua_tostring(_lua, -1));
    return -1;    
  }
  else _lastErrorMessage[0] = 0;
  return 1;
} 

int luaScript::callLuaFunc(const char *funcName, const luaObj &p0, const luaObj &p1){
  lua_settop(_lua, 0);  
  lua_getglobal(_lua, funcName);
  if(lua_isnoneornil(_lua, -1)) return 0;

  p0.toLuaStack(_lua);
  p1.toLuaStack(_lua);

  if(lua_pcall(_lua, 2, -1, 0)){
    strcpy_s(_lastErrorMessage, lua_tostring(_lua, -1));
    return -1;    
  }
  else _lastErrorMessage[0] = 0;
  return 1;
}     

int luaScript::callLuaFunc(const char *funcName, const luaObj &p0, const luaObj &p1, const luaObj &p2){
  lua_settop(_lua, 0);  
  lua_getglobal(_lua, funcName);
  if(lua_isnoneornil(_lua, -1)) return 0;

  p0.toLuaStack(_lua);
  p1.toLuaStack(_lua);
  p2.toLuaStack(_lua);

  if(lua_pcall(_lua, 3, -1, 0)){
    strcpy_s(_lastErrorMessage, lua_tostring(_lua, -1));
    return -1;    
  }
  else _lastErrorMessage[0] = 0;
  return 1;
}               

int luaScript::callLuaFunc(const char *funcName, const luaObj &p0, const luaObj &p1, const luaObj &p2, const luaObj &p3){
  lua_settop(_lua, 0);  
  lua_getglobal(_lua, funcName);
  if(lua_isnoneornil(_lua, -1)) return 0;

  p0.toLuaStack(_lua);
  p1.toLuaStack(_lua);
  p2.toLuaStack(_lua);
  p3.toLuaStack(_lua);

  if(lua_pcall(_lua, 4, -1, 0)){
    strcpy_s(_lastErrorMessage, lua_tostring(_lua, -1));
    return -1;    
  }
  else _lastErrorMessage[0] = 0;
  return 1;
}              

int luaScript::stackCount(){
  return lua_gettop(_lua);
}

luaObj luaScript::fromStack(int index) const{
  return luaObj(_lua, -lua_gettop(_lua) + index);
}

void luaScript::pushValue(const luaObj &val){
  val.toLuaStack(_lua);
}

template <class C> bool luaScript::pushObject(const char *meta, C *obj){
  C **ppObj = (C **)lua_newuserdata(_lua, sizeof(C *));
  *ppObj = obj;

  int stackTop = lua_gettop(_lua);
  luaL_getmetatable(_lua, meta);

  if(!lua_istable(_lua, -1)){   
    lua_settop(_lua, stackTop);
    return false;
  }

  lua_setmetatable(_lua, -2);
  return true;
}

void luaScript::setUserData(void *idata, int size){                   
  if(_userdata){
    free(_userdata);
    _userdata = 0;
    _userdataSize = 0;
  }
  if(size){
    _userdata = malloc(size);
    _userdataSize = size;
    memcpy(_userdata, idata, size);
  }
}

void luaScript::getUserData(void *odata){                              
  memcpy(odata, _userdata, _userdataSize);
}

int luaScript::getUserDataSize(){                                     
  return _userdataSize;
}

const char *luaScript::getLastError(){                                         
  return _lastErrorMessage;
}

bool luaScript::regMeta(const char *meta){
  bool ret;
  int stackTop = lua_gettop(_lua);

  luaL_getmetatable(_lua, meta);
  if(ret = !lua_istable(_lua, -1)){   
    lua_pop(_lua, 1);
    luaL_newmetatable(_lua, meta);
    lua_pushstring(_lua, "__index");  
    lua_newtable(_lua);         
    lua_rawset(_lua, -3);  
  }   
  lua_settop(_lua, stackTop);
  return ret;
}

template <class C> bool luaScript::regInstanceObject(const char *instName, const char *meta, C *obj){
  bool ret;
  int stackTop = lua_gettop(_lua);     
  C **ppObj = (C **)lua_newuserdata(_lua, sizeof(C *));
  *ppObj = obj;

  luaL_getmetatable(_lua, meta);
  if(ret = !lua_istable(_lua, -1)){   
    lua_pop(_lua, 1);
    luaL_newmetatable(_lua, meta);
    lua_pushstring(_lua, "__index");  
    lua_newtable(_lua);         
    lua_rawset(_lua, -3);  
  } 

  lua_setmetatable(_lua, -2);
  lua_setglobal(_lua, instName);

  lua_settop(_lua, stackTop);
  return ret;
}

template <class C> bool luaScript::regStdMethod(const char *meta, const char *mName, int (C::*pf)(luaScript *)){
  int stackTop = lua_gettop(_lua);              
  luaL_getmetatable(_lua, meta);

  if(!lua_istable(_lua, -1)){
    sprintf_s(_lastErrorMessage, "luaScript error: can't find metatable %s", meta);      
    return false;
  }                       
  lua_pushstring(_lua, "__index");
  lua_rawget(_lua, -2);
  lua_pushstring(_lua, mName);

  auto savedPF = (int (C::**)(luaScript *))lua_newuserdata(_lua, sizeof(decltype(pf)));
  *savedPF = pf;
  lua_pushlightuserdata(_lua, (void *)this);
  lua_pushcclosure(_lua, lsClassBinder::stdMethod <C>, 2);
  lua_rawset(_lua, -3);       
  lua_settop(_lua, stackTop);    

  return true;
}

template <class C> void luaScript::regClass(const char *meta, bool regDestructor){
  lsRegClass(_lua, meta, lsClassBinder::constructor <C>);
}

template <class C, typename P1> void luaScript::regClass(const char *meta, bool regDestructor){
  lsRegClass(_lua, meta, lsClassBinder::constructor <C, P1>);
}

template <class C, typename P1, typename P2> void luaScript::regClass(const char *meta, bool regDestructor){
  lsRegClass(_lua, meta, lsClassBinder::constructor <C, P1, P2>);
}

template <class C, typename P1, typename P2, typename P3> void luaScript::regClass(const char *meta, bool regDestructor){
  lsRegClass(_lua, meta, lsClassBinder::constructor <C, P1, P2, P3>);
}

template <class C, typename P1, typename P2, typename P3, typename P4> void luaScript::regClass(const char *meta, bool regDestructor){
  lsRegClass(_lua, meta, lsClassBinder::constructor <C, P1, P2, P3, P4>);
}

template <typename T> void luaScript::regFunction(const char *funcName, T (*f)()){
  lsRegFunction(_lua, funcName, f, lsFunctionBinder<T>::lsFunc);
}

template <typename T, typename P1> void luaScript::regFunction(const char *funcName, T (*f)(P1)){
  lsRegFunction(_lua, funcName, f, lsFunctionBinder<T>::lsFunc <P1>);
}

template <typename T, typename P1, typename P2> void luaScript::regFunction(const char *funcName, T (*f)(P1, P2)){
  lsRegFunction(_lua, funcName, f, lsFunctionBinder<T>::lsFunc <P1, P2>);
}

template <typename T, typename P1, typename P2, typename P3> void luaScript::regFunction(const char *funcName, T (*f)(P1, P2, P3)){
  lsRegFunction(_lua, funcName, f, lsFunctionBinder<T>::lsFunc <P1, P2, P3>);
}

template <typename T, typename P1, typename P2, typename P3, typename P4> void luaScript::regFunction(const char *funcName, T (*f)(P1, P2, P3, P4)){
  lsRegFunction(_lua, funcName, f, lsFunctionBinder<T>::lsFunc <P1, P2, P3, P4>);
}

template <typename T, class C> bool regMethod(const char *meta, const char *mName, T (C::*pf)()){
  return lsRegMethod(meta, mName, pf, lsClassBinder::MethodBinder <T>::lsFunc <C>);
}  

template <typename T, class C, typename P1> bool regMethod(const char *meta, const char *mName, T (C::*pf)(P1)){
  return lsRegMethod(meta, mName, pf, lsClassBinder::MethodBinder <T>::lsFunc <C, P1>);
}

template <typename T, class C, typename P1, typename P2> bool regMethod(const char *meta, const char *mName, T (C::*pf)(P1, P2)){
  return lsRegMethod(meta, mName, pf, lsClassBinder::MethodBinder <T>::lsFunc <C, P1, P2>);
}

template <typename T, class C, typename P1, typename P2, typename P3> bool regMethod(const char *meta, const char *mName, T (C::*pf)(P1, P2, P3)){  
  return lsRegMethod(meta, mName, pf, lsClassBinder::MethodBinder <T>::lsFunc <C, P1, P2, P3>);
}

template <typename T, class C, typename P1, typename P2, typename P3, typename P4> bool regMethod(const char *meta, const char *mName, T (C::*pf)(P1, P2, P3, P4)){
  return lsRegMethod(meta, mName, pf, lsClassBinder::MethodBinder <T>::lsFunc <C, P1, P2, P3, P4>);
}
