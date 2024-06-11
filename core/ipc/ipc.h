#pragma once

#include "tools/mainipc/types.h"
#include "utility/utility.h"

#include <functional>

class ICommunication {
  CLASS_NO_COPY(ICommunication);
  CLASS_NO_MOVE(ICommunication);

  public:
  typedef std::function<void(uint32_t id, uint32_t size, const char* data)> PHandler;

  ICommunication()          = default;
  virtual ~ICommunication() = default;

  virtual void write(IPCHeader* packet)   = 0;
  virtual void addHandler(PHandler pfunc) = 0;
  virtual void runReadLoop()              = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL std::unique_ptr<ICommunication> createIPC(const char* name);
#undef __APICALL
