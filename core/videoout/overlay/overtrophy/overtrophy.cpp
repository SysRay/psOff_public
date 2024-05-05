#include "overtrophy.h"

#include "core/trophies/trophies.h"

#include <imgui/imgui.h>

class OverTrophy: public IOverTrophy {
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

  std::vector<notify> m_notifications = {};

  ImFont* m_defaultFont;
  ImFont* m_titleFont;
  ImFont* m_textFont;

  ImFont* CreateFont(float px) {
    ImGuiIO&     io = ImGui::GetIO();
    ImFontConfig cfg;
    cfg.SizePixels  = px;
    cfg.OversampleH = cfg.OversampleV = 1;
    cfg.PixelSnapH                    = true;

    return io.Fonts->AddFontDefault(&cfg);
  }

  public:
  OverTrophy() {
    m_defaultFont = CreateFont(13);
    m_titleFont   = CreateFont(20);
    m_textFont    = CreateFont(15);

    accessTrophies().addTrophyUnlockCallback([this](const void* data) {
      auto unlock = (ITrophies::trp_unlock_data*)data;
      addNotify(unlock->grade, unlock->name, unlock->descr);
      if (unlock->platGained) addNotify('p', unlock->pname, unlock->pdescr);
    });
  }

  void addNotify(uint8_t grade, std::string title, std::string details) {
    m_notifications.emplace_back(0.0f, 5.0f, false, grade, std::move(title), std::move(details), nullptr, 0ull);
  }

  void draw(double fps) final {
    fps = std::max(1.0, fps);

    const double delta    = (1 / fps);
    const float  dsw      = ImGui::GetIO().DisplaySize.x;
    const float  slidemod = delta * 2.78f;
    float        wpos     = 0.0f;

    for (auto it = m_notifications.begin(); it != m_notifications.end();) {
      auto& notify = *it;

      if (notify.animdone == false) {
        if (notify.slide < 1.0f) {
          notify.slide += slidemod;
        } else if (notify.slide > 1.0f) {
          notify.animdone = true;
          notify.slide    = 1.0f;
        }
      } else {
        if ((notify.timer -= delta) < 0.0f) {
          if (notify.slide > 0.0f) {
            notify.slide -= slidemod;
          } else {
            it = m_notifications.erase(it);
            continue;
          }
        }
      }

      const char* title;
      switch (notify.grade) {
        case 'b': {
          title = "Bronze trophy unlocked!";
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(199, 124, 48, 255));
        } break;
        case 's': {
          title = "Silver trophy unlocked!";
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 164, 182, 255));
        } break;
        case 'g': {
          title = "Gold trophy unlocked!";
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(228, 194, 78, 255));
        } break;
        case 'p': {
          title = "Platinum trophy unlocked!";
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(191, 191, 191, 255));
        } break;
      }

      ImGui::PushFont(m_defaultFont);
      ImGui::SetNextWindowPos(ImVec2(dsw - 360 * notify.slide, wpos + 10.0f));
      ImGui::SetNextWindowSize(ImVec2(350, 0));
      ImGui::Begin(title, nullptr,
                   ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoFocusOnAppearing);
      ImGui::PopStyleColor();
      ImGui::PushFont(m_titleFont);
      ImGui::Text("%s", notify.title.c_str());
      ImGui::PopFont();

      ImGui::PushFont(m_textFont);
      ImGui::TextWrapped("%s", notify.subtext.c_str());
      ImGui::PopFont();

      wpos += ImGui::GetWindowHeight() + 10.0f;
      ImGui::End();
      ImGui::PopFont();
      ++it;
    }
  }
};

IOverTrophy& accessTrophyOverlay() {
  static OverTrophy ot;
  return ot;
}
