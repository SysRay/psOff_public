#pragma once

#include "utility/utility.h"

#define GAMEREPORT_REPO_NAME "SysRay/psOff_compatibility"

class IGameReport {
  CLASS_NO_COPY(IGameReport);
  CLASS_NO_MOVE(IGameReport);

  protected:
  bool m_bEnabled = true;

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
    const void*     ptr;
  };

  struct Info {
    const char* title;
    const char* title_id;
    const char* app_ver;
    const char* emu_ver;

    Type           type;
    AdditionalData add;
  };

  void SetStatus(bool enabled) { m_bEnabled = enabled; };

  virtual void ShowReportWindow(const Info&) = 0;
};

#ifdef __APICALL_EXTERN
#define __APICALL __declspec(dllexport)
#else
#define __APICALL __declspec(dllimport)
#endif

__APICALL IGameReport& accessGameReport();
#undef __APICALL
