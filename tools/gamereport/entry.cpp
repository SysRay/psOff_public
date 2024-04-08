#define __APICALL_EXTERN
#include "gamereport.h"
#undef __APICALL_EXTERN

#include "config_emu.h"
#include "git_ver.h"
#include "logging.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <boost/url.hpp>
#include <format>
#include <shellapi.h>

LOG_DEFINE_MODULE(GameReport)

class GameReport: public IGameReport {
  public:
  GameReport();
  virtual ~GameReport() = default;

  virtual void ShowReportWindow(const Info& info) final;
};

GameReport::GameReport() {}

void GameReport::ShowReportWindow(const Info& info) {
  LOG_USE_MODULE(GameReport);
  if (m_bEnabled == false) {
    LOG_WARN(L"[gamereport] Reporting is disabled!");
    return;
  }

  const char* message;

  switch (info.type) {
    case USER:
      message = "Do you want to file report about the game you running?\n"
                "If you press \"Yes\", your default browser will be opened.\n\n"
                "You must have a GitHub account to create an issue!";
      break;

    case EXCEPTION:
    case MISSING_SYMBOL:
      message = "Looks like your emulator just crashed! Do you want to file a game report?\n"
                "If you press \"Yes\", your default browser will be opened.\n\n"
                "You must have a GitHub account to create an issue!";
      break;

    default: break;
  }

  SDL_MessageBoxButtonData btns[2] {
      {.flags = 0, .buttonid = 1, .text = "Yes"},
      {.flags = 0, .buttonid = 2, .text = "No"},
  };

  SDL_MessageBoxData mbd {
      .flags      = SDL_MESSAGEBOX_INFORMATION,
      .window     = info.wnd,
      .title      = "File a game report",
      .message    = message,
      .numbuttons = 2,
      .buttons    = btns,
  };

  int btn = -1;

  if (SDL_ShowMessageBox(&mbd, &btn) != 0) {
    LOG_ERR(L"[gamereport] Failed to generate SDL MessageBox: %S", SDL_GetError());
    return;
  }

  if (btn != 1) return;

  try {
    std::string str;
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::LOGGING);
    (*jData)["sink"].get_to(str);
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });

    if (str != "filebin") {
      mbd.title      = "Logging warning";
      mbd.message    = "You might not be able to provide a log for issue you about to open.\n"
                       "Your logger configuration might be incorrect, since \"sink\" is not equals to \"FileBin\".\n"
                       "Creating issues without log file is not recommended.";
      mbd.flags      = SDL_MESSAGEBOX_WARNING;
      mbd.numbuttons = 2;
      btns[0].text   = "OK";
      btns[1].text   = "Cancel";

      if (SDL_ShowMessageBox(&mbd, &btn) != 0) {
        LOG_ERR(L"[gamereport] Failed to generate SDL MessageBox: %S", SDL_GetError());
        return;
      }

      if (btn != 1) return;
    }
  } catch (const json::exception& ex) {
  }

  boost::urls::url link("https://github.com/SysRay/psOff_compatability/issues/new");

  auto params = link.params();
  params.append({"template", "game_report.yml"});
  switch (info.type) {
    case EXCEPTION:
      params.append({"what-happened", info.add.ex->what()}); // todo: add stack trace?
      break;
    case MISSING_SYMBOL:
      params.append({"what-happened", info.add.message}); // todo: some formatting?
      break;

    default: break;
  }
  params.append({"title", std::format("[{}]: {}", info.title_id, info.title)});
  params.append({"game-version", info.app_ver});
  params.append({"lib-version", git::CommitSHA1()});
  link.normalize();

  ShellExecuteA(nullptr, nullptr, link.buffer().data(), nullptr, nullptr, SW_SHOW);
}

IGameReport& accessGameReport() {
  static GameReport obj;
  return obj;
}
