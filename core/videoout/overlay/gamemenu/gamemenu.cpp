#include "gamemenu.h"

#include "core/hotkeys/hotkeys.h"
#include "core/trophies/trophies.h"

#include <chrono>
#include <imgui/imgui.h>

void GameMenu::init() {
  accessHotkeys().registerCallback(HkCommand::OVERLAY_MENU, [this](HkCommand) { toggle(); });

  m_buttons.emplace_back("Show trophies list", [this]() { switchTo(MenuState::TROPHIES); });
  m_buttons.emplace_back("Close this menu", [this]() { toggle(); });
}

void GameMenu::_PushTrophy(int32_t trophyId, std::string& name, std::string& detail) {
  m_trophyList.emplace_back(trophyId, name, detail);
}

void GameMenu::switchTo(MenuState state) {
  switch (m_state) { // Close the previous state
    case MenuState::TROPHIES: {
      m_trophyList.clear();
    } break;

    default: break;
  }

  switch (state) {
    case MenuState::MAIN: break; // We have nothing to do when switching to main
    case MenuState::TROPHIES: {
      ITrophies::trp_context ctx = {
          .entry =
              {
                  .func = [this](ITrophies::trp_ent_cb::data_t* data) -> bool {
                    _PushTrophy(data->id, data->name, data->detail);
                    return false;
                  },
              },
      };

      if (accessTrophies().parseTRP(&ctx) != ITrophies::ErrCodes::SUCCESS) {
        m_trophyList.clear();
      }
    } break;
  }

  m_state = state;
}

void GameMenu::_DrawTrophiesFor(int32_t userId) {
  struct TrophyUnlock {
    int32_t     id;
    std::string time;
  };

  static int32_t                   prev_user = -1;
  static std::vector<TrophyUnlock> unlocks   = {};

  auto& trophies = accessTrophies();

  if (prev_user != userId) {
    prev_user = userId;
    unlocks.clear();
    if (trophies.createContext(userId, 0) == 0) {
      for (auto& trophy: m_trophyList) {
        if (auto utime = trophies.getUnlockTime(userId, trophy.id)) {
          auto tp = std::chrono::system_clock::from_time_t(utime);
          unlocks.emplace_back(trophy.id, std::format("Unlocked: {0:%m %d %Y %H:%M:%S}", tp));
        }
      }
    }
  }

  for (auto& trophy: m_trophyList) {
    auto it = unlocks.begin();
    for (; it != unlocks.end(); ++it) {
      if (it->id == trophy.id) {
        break;
      }
    }

    const char* status = "Locked";

    if (it != unlocks.end()) status = it->time.c_str();

    ImGui::Text("%s\n%s\n%s", trophy.title.c_str(), trophy.subtitle.c_str(), status);
    ImGui::Separator();
  }
}

void GameMenu::draw() {
  if (!m_bShown) return;

  auto&      io = ImGui::GetIO();
  auto const ds = io.DisplaySize;

  static const ImVec2 btn_size(150.0f, ImGui::GetFrameHeightWithSpacing());
  static const ImVec2 win_size(400.0f, 320.0f);
  static const float  win_center_x((win_size.x - btn_size.x) * 0.5f);

  ImGui::SetNextWindowSize(win_size);
  ImGui::SetNextWindowPos(ImVec2(ds.x / 2.0f - 200.0f, ds.y / 2.0f - 160.0f));
  ImGui::Begin("ingame_menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

  static const char* names[] = {"User #1", "User #2", "User #3", "User #4"};

  switch (m_state) {
    case MenuState::MAIN: {
      if (auto btncnt = m_buttons.size()) {
        ImGui::SetCursorPosY((win_size.y - btncnt * btn_size.y) * 0.5f);

        for (auto& btn: m_buttons) {
          ImGui::SetCursorPosX(win_center_x);
          if (ImGui::Button(btn.title.c_str(), btn_size)) {
            btn.func();
          }
        }
      }
    } break;

    case MenuState::TROPHIES: {
      if (ImGui::BeginTabBar("trophies_tab")) {
        for (int i = 0; i < 4; ++i) {
          if (ImGui::BeginTabItem(names[i])) {
            static const ImVec2 child_size(0, win_size.y - btn_size.y - 45.0f);
            ImVec2              backbtn_size(ImGui::GetContentRegionAvail().x, btn_size.y);
            ImGui::BeginChild("user_trophies", child_size, ImGuiChildFlags_Border);
            _DrawTrophiesFor(i + 1);
            ImGui::EndChild();
            if (ImGui::Button("Go back", backbtn_size)) {
              switchTo(MenuState::MAIN);
            }
            ImGui::EndTabItem();
          }
        }

        ImGui::EndTabBar();
      }

    } break;
  }

  ImGui::End();
};
