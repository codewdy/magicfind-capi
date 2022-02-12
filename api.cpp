#include "pch.h"

#ifdef WIN32
#define EXPORT_FUNC __declspec(dllexport)
#else
#define EXPORT_FUNC
#endif

extern "C" {
  EXPORT_FUNC const char* error_message();
  EXPORT_FUNC int init(const char* rootdir);
  EXPORT_FUNC int run_one_frame();
  EXPORT_FUNC void get_frame_result_simple(struct FrameResultSimple*);
  EXPORT_FUNC void get_frame_result_units(struct Unit*);
  EXPORT_FUNC void get_frame_result_new_effects(struct Effect*);
  EXPORT_FUNC int get_unit_prototype(int handle, struct UnitPrototype* prototype);
  EXPORT_FUNC int get_effect_prototype(int handle, struct EffectPrototype* prototype);
  EXPORT_FUNC int init_logger(int level, const char* filename);
}

#include "lua_interface.h"

const char* error_message() {
  return LuaInterface::Instance()->error_message();
}
int init(const char* rootdir) {
  return LuaInterface::Instance()->init(rootdir);
}
FrameResult frame_result;
int run_one_frame() {
  return LuaInterface::Instance()->run_function("RunOneFrame", &frame_result);
}
void get_frame_result_simple(struct FrameResultSimple* result) {
  result->state = frame_result.state;
  result->units_size = frame_result.units.size();
  result->new_effects_size = frame_result.new_effects.size();
}
void get_frame_result_units(struct Unit* result) {
  for (int i = 0; i < frame_result.units.size(); i++) {
    result[i] = frame_result.units[i];
  }
}
void get_frame_result_new_effects(struct Effect* result) {
  for (int i = 0; i < frame_result.new_effects.size(); i++) {
    result[i] = frame_result.new_effects[i];
  }
}
int get_unit_prototype(int handle, struct UnitPrototype* prototype) {
  return LuaInterface::Instance()->run_function(
      "GetUnitPrototype", prototype, handle);
}
int get_effect_prototype(int handle, struct EffectPrototype* prototype) {
  return LuaInterface::Instance()->run_function(
      "GetEffectPrototype", prototype, handle);
}
int init_logger(int level, const char* filename) {
  return LuaInterface::Instance()->run_function(
      "InitLogger", nullptr, filename, level);
}

#ifdef TESTMAIN

int main(int argc, const char** argv) {
  if (init(argv[argc - 1])) {
    std::cerr << "INIT ERROR: " << error_message() << std::endl;
    return -1;
  }
  if (init_logger(1, "magicfind.log")) {
    std::cerr << "INIT LOGGER ERROR: " << error_message() << std::endl;
    return -1;
  }
  for (int i = 0; i < 1000; i++) {
    FrameResult fr;
    if (run_one_frame()) {
      std::cerr << "RunOneFrame ERROR: " << error_message() << std::endl;
      return -1;
    }
    FrameResultSimple frs;
    get_frame_result_simple(&frs);
    std::cerr << frs.state << std::endl;
    std::vector<Unit> units(frs.units_size);
    get_frame_result_units(&units[0]);
#ifndef NOOUTPUT
    std::cerr << "Units:";
    for (auto&& item : units) {
      UnitPrototype prototype;
      get_unit_prototype(item.prototype_handle, &prototype);
      std::cerr << prototype.icon << "," << prototype.size << "," << item.hp << "/" << item.maxHp << " ";
    }
    std::cerr << std::endl;
#endif
    std::vector<Effect> effects(frs.new_effects_size);
    get_frame_result_new_effects(&effects[0]);
#ifndef NOOUTPUT
    std::cerr << "Effects:";
    for (auto&& item : effects) {
      EffectPrototype prototype;
      get_effect_prototype(item.handle, &prototype);
      std::cerr << item.handle << "," << prototype.type << "," << prototype.config << ",";
      std::cerr << item.a << "," << item.b << "," << item.c << "," << item.d << ",";
      std::cerr << item.e << "," << item.f << "," << item.g << " ";
    }
    std::cerr << std::endl;
#endif
  }
}
#endif
