#pragma once
#include "utility/utility.h"

#include <string>

class IInitParams {
  CLASS_NO_COPY(IInitParams);
  CLASS_NO_MOVE(IInitParams);

  public:
  IInitParams() = default;

  virtual bool init(int argc, char** argv) = 0;
  virtual bool isDebug()                   = 0;

  virtual std::string getApplicationPath() = 0;
  virtual std::string getApplicationRoot() = 0;
  virtual std::string getUpdateRoot()      = 0;

  virtual bool enableValidation() = 0;
  virtual bool enableBrightness() = 0;
  virtual bool useVSYNC()         = 0;
  virtual bool try4K()            = 0;

  virtual ~IInitParams() = default;
};

#if defined(__APICALL_INITPARAMS_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL IInitParams* accessInitParams();
#undef __APICALL