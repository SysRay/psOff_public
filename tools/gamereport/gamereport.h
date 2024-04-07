#pragma once

#include "SDL.h"
#include "utility/utility.h"

class IGameReport {
  CLASS_NO_COPY(IGameReport);
  CLASS_NO_MOVE(IGameReport);

  protected:
  IGameReport() = default;

  public:
  virtual ~IGameReport() = default;

  struct GameReportInfo {
    const char* title;
    const char* title_id;
    const char* app_ver;
    SDL_Window* wnd;
  };

  virtual void ShowReportWindow(const GameReportInfo*) = 0;
};

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IGameReport& accessGameReport();
#undef __APICALL
