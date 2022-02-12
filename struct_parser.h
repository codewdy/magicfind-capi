#ifndef MAGICFIND_STRUCT_PARSER_H_
#define MAGICFIND_STRUCT_PARSER_H_

extern "C" {
#include "luajit.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string>
#include "lua_types.h"

namespace struct_parser {

struct LuaStackHolder {
  LuaStackHolder(lua_State* L) : L_(L) {}
  ~LuaStackHolder() {
    lua_pop(L_, 1);
  }
  lua_State* L_;
};

static void Parse(lua_State* L, int* result) {
  LuaStackHolder lsh(L);
  if (lua_isnumber(L, -1)) {
    *result = lua_tointeger(L, -1);
  }
}

static void Parse(lua_State* L, float* result) {
  LuaStackHolder lsh(L);
  if (lua_isnumber(L, -1)) {
    *result = lua_tonumber(L, -1);
  }
}

static void Parse(lua_State* L, const char** result) {
  LuaStackHolder lsh(L);
  if (lua_isstring(L, -1)) {
    *result = lua_tostring(L, -1);
  }
}

static void Parse(lua_State* L, std::string* result) {
  LuaStackHolder lsh(L);
  if (lua_isstring(L, -1)) {
    *result = lua_tostring(L, -1);
  }
}

static void Parse(lua_State* L, bool* result) {
  LuaStackHolder lsh(L);
  *result = lua_toboolean(L, -1);
}
template<typename T,
  typename std::enable_if<mirror::Enable<T>::value, bool>::type = true>
static void Parse(lua_State* L, T* result);

template<typename T>
static void Parse(lua_State* L, std::vector<T>* result) {
  LuaStackHolder lsh(L);
  if (!lua_istable(L, -1)) {
    return;
  }
  int size = 0;
  lua_getfield(L, -1, "size");
  Parse(L, &size);
  result->resize(size);
  lua_getfield(L, -1, "vec");
  LuaStackHolder lsh2(L);
  if (!lua_istable(L, -1)) {
    return;
  }
  lua_pushnil(L);
  while (lua_next(L, -2)) {
    if (lua_isnumber(L, -2)) {
      std::size_t id = lua_tonumber(L, -2);
      if (id <= result->size()) {
        Parse(L, &result->at(id - 1));
      } else {
        lua_pop(L, 1);
      }
    } else {
      lua_pop(L, 1);
    }
  }
}

struct ParserVisitor {
  ParserVisitor(lua_State* L) : L_(L) { }
  template<typename T>
  void run(const char* name, T& val) {
    lua_getfield(L_, -1, name);
    Parse(L_, &val);
  }
  lua_State* L_;
};

template<typename T,
  typename std::enable_if<mirror::Enable<T>::value, bool>::type>
static void Parse(lua_State* L, T* result) {
  LuaStackHolder lsh(L);
  if (!lua_istable(L, -1)) {
    return;
  }
  mirror::Reflection<T>::ForeachField(ParserVisitor(L), *result);
}

}

#endif
