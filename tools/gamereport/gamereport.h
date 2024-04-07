#pragma once

#include "utility/utility.h"
#include "SDL.h"

class IGameReport {
  CLASS_NO_COPY(IGameReport);
  CLASS_NO_MOVE(IGameReport);

  protected:
  IGameReport() = default;

  public:
  virtual ~IGameReport() = default;

  virtual void ShowReportWindow() = 0;
};

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IGameReport& accessGameReport();
#undef __APICALL
