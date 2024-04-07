#define __APICALL_EXTERN
#include "gamereport.h"
#undef __APICALL_EXTERN

#include "logging.h"

LOG_DEFINE_MODULE(GameReport)

class GameReport : public IGameReport {
  public:
  GameReport();
  virtual ~GameReport() = default;

  virtual void ShowReportWindow() final;
};

GameReport::GameReport() {}

void GameReport::ShowReportWindow() {
  LOG_USE_MODULE(GameReport);

  const SDL_MessageBoxButtonData btns[2] {
    {.flags = 0, .buttonid = 1, .text = "Yes"},
    {.flags = 0, .buttonid = 2, .text = "No"},
  };

  const SDL_MessageBoxData mbd {
    .flags = SDL_MESSAGEBOX_INFORMATION,
    .title = "File a game report",
    .message = "Do you want to file report about the game you running?\n"
               "If you press \"Yes\", your default browser will be opened.",
    .numbuttons = 2,
    .buttons = btns,
  };
  int btn = -1;

  if (SDL_ShowMessageBox(&mbd, &btn) != 0) {
    
    return;
  }
}

IGameReport& accessGameReport() {
  static GameReport obj;
  return obj;
}
