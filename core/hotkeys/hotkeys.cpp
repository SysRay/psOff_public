#define __APICALL_EXTERN
#include "hotkeys.h"
#undef __APICALL_EXTERN

#include "config_emu.h"

#include <unordered_map>

class Hotkeys: public IHotkeys {
  private:
  struct HkBacks {
    HkCommand cmd;
    HkFunc    func;
  };

  struct KBind {
    SDL_Keycode kc;
    uint16_t    mod;
  };

  std::vector<HkBacks>                               m_cbacks = {};
  std::array<KBind, (size_t)HkCommand::COMMANDS_MAX> m_binds  = {{SDLK_UNKNOWN, 0}};
  uint32_t                                           m_state  = 0;

  HkCommand lookupCommand(SDL_Keycode kc, uint16_t mod) {
    for (uint32_t c = uint32_t(HkCommand::UNKNOWN); c < uint32_t(HkCommand::COMMANDS_MAX); ++c) {
      auto& bind = m_binds[(uint32_t)c];
      if (kc == bind.kc && mod == bind.mod) return HkCommand(c);
    }

    return HkCommand::UNKNOWN;
  }

  public:
  Hotkeys() {
    static std::unordered_map<std::string, HkCommand> map = {
        {"gamereport.send", HkCommand::GAMEREPORT_SEND_REPORT},
        {"controller.l3", HkCommand::CONTROLLER_L3},
        {"controller.r3", HkCommand::CONTROLLER_R3},
        {"controller.options", HkCommand::CONTROLLER_OPTIONS},
        {"controller.dpad_up", HkCommand::CONTROLLER_DPAD_UP},
        {"controller.dpad_right", HkCommand::CONTROLLER_DPAD_RIGHT},
        {"controller.dpad_down", HkCommand::CONTROLLER_DPAD_DOWN},
        {"controller.dpad_left", HkCommand::CONTROLLER_DPAD_LEFT},
        {"controller.l2", HkCommand::CONTROLLER_L2},
        {"controller.r2", HkCommand::CONTROLLER_R2},
        {"controller.l1", HkCommand::CONTROLLER_L1},
        {"controller.r1", HkCommand::CONTROLLER_R1},
        {"controller.triangle", HkCommand::CONTROLLER_TRIANGLE},
        {"controller.circle", HkCommand::CONTROLLER_CIRCLE},
        {"controller.cross", HkCommand::CONTROLLER_CROSS},
        {"controller.square", HkCommand::CONTROLLER_SQUARE},
        {"controller.touchpad", HkCommand::CONTROLLER_TOUCH_PAD},
        {"controller.lx+", HkCommand::CONTROLLER_LX_UP},
        {"controller.lx-", HkCommand::CONTROLLER_LX_DOWN},
        {"controller.ly+", HkCommand::CONTROLLER_LY_UP},
        {"controller.ly-", HkCommand::CONTROLLER_LY_DOWN},
        {"controller.rx+", HkCommand::CONTROLLER_RX_UP},
        {"controller.rx-", HkCommand::CONTROLLER_RX_DOWN},
        {"controller.ry+", HkCommand::CONTROLLER_RY_UP},
        {"controller.ry-", HkCommand::CONTROLLER_RY_DOWN},
    };

    auto readBind = [](std::string& str, KBind* kb) -> bool {
      auto delim = str.find_last_of("||");
      if (delim != std::string::npos) {
        auto mod = str.substr(0, delim - 1);
        auto key = str.substr(delim + 1);
        kb->kc   = SDL_GetScancodeFromName(key.c_str());

        switch (SDL_GetScancodeFromName(mod.c_str())) {
          case SDL_SCANCODE_LALT: kb->mod = KMOD_LALT; return true;
          case SDL_SCANCODE_LCTRL: kb->mod = KMOD_LCTRL; return true;
          case SDL_SCANCODE_LSHIFT: kb->mod = KMOD_LSHIFT; return true;

          case SDL_SCANCODE_RALT: kb->mod = KMOD_RALT; return true;
          case SDL_SCANCODE_RCTRL: kb->mod = KMOD_RCTRL; return true;
          case SDL_SCANCODE_RSHIFT: kb->mod = KMOD_RSHIFT; return true;

          default: return false;
        }
      }

      kb->kc  = SDL_GetScancodeFromName(str.c_str());
      kb->mod = 0x0000;
      return true;
    };

    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::CONTROLS);
    for (auto& [bind, sdlkeys]: (*jData)["keybinds"].items()) {
      auto it = map.find(bind.c_str());
      if (it == map.end()) continue;
      std::string temp;
      sdlkeys.get_to(temp);

      auto& bind = m_binds[(uint32_t)it->second];
      if (!readBind(temp, &bind)) bind = {0, 0};
    }
  }

  void registerCallback(HkCommand cmd, HkFunc func) final { m_cbacks.emplace_back(cmd, func); }

  bool isPressed(HkCommand cmd) final { return (m_state & (1ull << (uint8_t)cmd)) != 0; }

  int32_t isPressedEx(HkCommand cmd1, HkCommand cmd2, int32_t ifcmd1, int32_t ifcmd2, int32_t def) final {
    return isPressed(cmd1) ? ifcmd1 : (isPressed(cmd2) ? ifcmd2 : def);
  }

  void update(const SDL_KeyboardEvent* event) final {
    bool      pressed = event->type == SDL_KEYDOWN;
    HkCommand cmd     = lookupCommand(event->keysym.scancode, event->keysym.mod & 0x03C3);
    if (cmd == HkCommand::UNKNOWN) return;
    uint32_t bit = (1ull << (uint8_t)cmd);

    if (pressed && ((m_state & bit) == 0)) {
      for (auto& hkc: m_cbacks)
        if (hkc.cmd == cmd) hkc.func(cmd);

      m_state |= bit;
    } else if (!pressed && ((m_state & bit) != 0)) {
      m_state &= ~bit;
    }
  }
};

IHotkeys& accessHotkeys() {
  static Hotkeys ih;
  return ih;
}
