#pragma once

#include "external/libSceIme/types.h"
#include "utility/utility.h"

class IOscCtl {
  CLASS_NO_COPY(IOscCtl);
  CLASS_NO_MOVE(IOscCtl);

  public:
  enum class Status {
    HIDDEN,
    SHOWN,
    CLOSED,
  };

  struct Params {
    Status status;
    char   title[32];
    char   details[32];
    char   enterLabel[32];

    char*  buffer;
    size_t buffersz;

    float xpos, ypos;
  };

  IOscCtl()          = default;
  virtual ~IOscCtl() = default;

  virtual Params* getParams() = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL IOscCtl& accessOscCtl();
#undef __APICALL
