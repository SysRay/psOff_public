#pragma once

#include "config_emu.h"
#include "types.h"

class ControllerConfig {
  struct Key {
    uint16_t key;
    uint16_t mod;
  } m_keymap[ControllerKey::CONTROLLER_KEYS_COUNT];

  struct DeadZone {
    float x, y;
  };

  struct Settings {
    ControllerType type;
    DeadZone       ls, rs;
  } m_pads[4];

  public:
  auto GetPadType(int n) const { return m_pads[n].type; }

  auto GetBind(ControllerKey key) const { return m_keymap[key]; }

  ControllerConfig();
  virtual ~ControllerConfig() = default;
};
