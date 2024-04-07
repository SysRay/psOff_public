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

  enum Type {
    USER,
    EXCEPTION,
    MISSING_SYMBOL,
  };

  union AdditionalData {
    std::exception* ex;
    const char*     message;
  };

  struct Info {
    const char* title;
    const char* title_id;
    const char* app_ver;
    SDL_Window* wnd;

    Type           type;
    AdditionalData add;
  };

  virtual void ShowReportWindow(const Info&) = 0;
};

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IGameReport& accessGameReport();
#undef __APICALL
