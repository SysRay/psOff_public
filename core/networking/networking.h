#pragma once

#include "modules/libSceNet/resolverTypes.h"
#include "modules/libSceNet/socketTypes.h"
#include "modules/libSceNetCtl/types.h"
#include "modules_include/common.h"
#include "utility/utility.h"

class INetworking {
  CLASS_NO_COPY(INetworking);
  CLASS_NO_MOVE(INetworking);

  protected:
  INetworking() {}

  public:
  virtual ~INetworking() = default;

  /* SceNetCtl facility*/

  /**
   * @brief gets information about network parameters
   *
   * @param code
   * @param info
   * @return int32_t
   */
  virtual int32_t netCtlGetInfo(int code, SceNetCtlInfo* info) = 0;

  /* SceNet facility */

  /* Resolver sub-facility */
  virtual int32_t resolverCreate(const char* name, int memid, int flags) = 0;
  virtual int32_t resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) = 0;
  virtual int32_t resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) = 0;
  virtual int32_t resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) = 0;
  virtual int32_t resolverGetError(SceNetId rid, int* result) = 0;
  virtual int32_t resolverDestroy(SceNetId rid) = 0;
  virtual int32_t resolverAbort(SceNetId rid, int flags) = 0;

  /* Epoll sub-facility */
  // todo

  /* Socket sub-facility */
  // guess what? todo.
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL INetworking& accessNetworking();
#undef __APICALL
