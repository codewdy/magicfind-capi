#ifndef MAGICFIND_LUA_INTERFACE_H_
#define MAGICFIND_LUA_INTERFACE_H_

extern "C" {
#include "luajit.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "struct_parser.h"
#include <vector>

class LuaInterface {
 public:
  static LuaInterface* Instance() {
    static LuaInterface instance;
    return &instance;
  }
  lua_State* State() { return L; }

  int init(const char* rootdir) {
    std::string path = rootdir;
    path.append("/");
    path.append("?.lua");
    std::string file = rootdir;
    file.append("/");
    file.append("main.lua");
    L = luaL_newstate();
    luaopen_jit(L);
    luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
    luaL_openlibs(L);
    set_lua_path(path.c_str());
    if (luaL_loadfile(L, file.c_str()) != 0) {
      error_message_ = lua_tostring(L, -1);
      lua_pop(L, 1);
      return -1;
    }
    if (lua_pcall(L, 0, 0, 0) != 0) {
      error_message_ = lua_tostring(L, -1);
      lua_pop(L, 1);
      return -1;
    }
    return 0;
  }

  int set_lua_path(const char* path) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    std::string cur_path = lua_tostring(L, -1);
    cur_path.append(";");
    cur_path.append(path);
    lua_pop(L, 1);
    lua_pushstring(L, cur_path.c_str());
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);
    return 0;
  }

  int push(int x) {
    lua_pushinteger(L, x);
    return 0;
  }

  int push(float x) {
    lua_pushnumber(L, x);
    return 0;
  }

  int push(const char* x) {
    lua_pushstring(L, x);
    return 0;
  }

  const char* error_message() {
    return error_message_.c_str();
  }

  template<typename... Tin, typename Tout>
  int run_function(const char* func, Tout* out, Tin... args) {
    lua_getglobal(L, func);
    std::vector<int> helper{push(args)...};
    if (lua_pcall(L, sizeof...(Tin), 1, 0) != 0) {
      error_message_ = lua_tostring(L, -1);
      lua_pop(L, 1);
      return -1;
    }
    struct_parser::Parse(L, out);
    return 0;
  }

  template<typename... Tin>
  int run_function(const char* func, std::nullptr_t no_out, Tin... args) {
    lua_getglobal(L, func);
    std::vector<int> helper{push(args)...};
    if (lua_pcall(L, sizeof...(Tin), 1, 0) != 0) {
      error_message_ = lua_tostring(L, -1);
      lua_pop(L, 1);
      return -1;
    }
    return 0;
  }

 private:
  std::string error_message_;
  lua_State* L;
};

#endif
