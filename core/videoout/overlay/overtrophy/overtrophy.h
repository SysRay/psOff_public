#pragma once

#include <imgui/imgui.h>
#include <string>
#include <vector>

class OverTrophy {
  struct notify {
    float slide    = 1.0f;
    float timer    = 5.0f;
    bool  animdone = false;

    uint8_t     grade;
    std::string title;
    std::string subtext;
    void*       pngdata;
    size_t      pngsize;

    ~notify() {
      if (pngdata != nullptr) ::free(pngdata);
    }
  };

  std::vector<notify> m_notifications;

  ImFont* m_defaultFont;
  ImFont* m_titleFont;
  ImFont* m_textFont;

  ImFont* CreateFont(float px);
  void    addNotify(uint8_t grade, std::string title, std::string details);

  public:
  OverTrophy()          = default;
  virtual ~OverTrophy() = default;

  void init();
  void draw(double fps);
};
