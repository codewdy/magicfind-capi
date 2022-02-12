#ifndef MAGIC_FIND_LUA_TYPES_H_
#define MAGIC_FIND_LUA_TYPES_H_

#include "mirror.h"

#include <vector>

struct Unit {
  int prototype_handle;
  float maxHp;
  float hp;
  float posX;
  float posY;
  bool death;
};
MIRROR_REFLECTION_DEFINE(Unit,
    prototype_handle, maxHp, hp, posX, posY, death);

struct Effect {
  int handle;
  float a, b, c, d, e, f, g;
};
MIRROR_REFLECTION_DEFINE(Effect,
    handle, a, b, c, d, e, f, g);

struct UnitPrototype {
  const char* icon;
  float size;
};
MIRROR_REFLECTION_DEFINE(UnitPrototype, icon, size)

struct EffectPrototype {
  const char* type;
  const char* config;
};
MIRROR_REFLECTION_DEFINE(EffectPrototype, type, config)

struct FrameResult {
  int state;
  std::vector<Unit> units;
  std::vector<Effect> new_effects;
};
MIRROR_REFLECTION_DEFINE(FrameResult, state, units, new_effects)

struct FrameResultSimple {
  int state;
  int units_size;
  int new_effects_size;
};

#endif
