#pragma once

#include "modules/libSceNet/resolverTypes.h"
#include "modules/libSceNet/socketTypes.h"
#include "modules/libSceNet/types.h"
#include "modules/libSceNetCtl/types.h"
#include "modules_include/common.h"
#include "utility/utility.h"

class INetworking {
  CLASS_NO_COPY(INetworking);
  CLASS_NO_MOVE(INetworking);
  static thread_local int g_net_errno;

  protected:
  INetworking() {}

  public:
  virtual ~INetworking() = default;

  /**
   * @brief returns current network error code
   * 
   * @return int32_t
   */
  static int32_t getLastError();

  /**
   * @brief returns errno ptr
   *
   * @return int*
   */
  static int* getErrnoPtr() { return &g_net_errno; };

  /* SceNetCtl facility*/

  /**
   * @brief returns information about specified network parameter
   *
   * @param code
   * @param info
   * @return int32_t
   */
  virtual int32_t netCtlGetInfo(int code, SceNetCtlInfo* info) = 0;

  /* SceNet facility */

  /* Resolver sub-facility */
  virtual SceNetId resolverCreate(const char* name, int memid, int flags) = 0;
  virtual int32_t resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) = 0;
  virtual int32_t resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) = 0;
  virtual int32_t resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) = 0;
  virtual int32_t resolverGetError(SceNetId rid, int* result) = 0;
  virtual int32_t resolverDestroy(SceNetId rid) = 0;
  virtual int32_t resolverAbort(SceNetId rid, int flags) = 0;

  /* Epoll sub-facility */
  virtual SceNetId epollCreate(const char* name, int flags) = 0;
  virtual int epollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) = 0;
  virtual int epollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) = 0;
  virtual int epollDestroy(SceNetId eid) = 0;
  virtual int epollAbort(SceNetId eid, int flags) = 0;

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
