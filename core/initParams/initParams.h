#pragma once
#include "utility/utility.h"

#include <string>

struct InitParamsPimpl;

class InitParams {
  CLASS_NO_COPY(InitParams);
  CLASS_NO_MOVE(InitParams);

  InitParamsPimpl* _pImpl = nullptr;

  public:
  InitParams();

  bool init(int argc, char** argv);

  // Functions
  bool isDebug();

  std::string getApplicationPath();
  std::string getApplicationRoot();

  bool enableValidation();
  bool useVSYNC();
  ~InitParams();
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL InitParams* accessInitParams();
#undef __APICALL