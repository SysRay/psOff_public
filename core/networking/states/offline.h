#pragma once
#include "../networking.h"

class OfflineNet: public INetworking {
  public:
  /* SceNetCtl facility*/

  /**
   * @brief gets information about network parameters
   *
   * @param code
   * @param info
   * @return int32_t
   */
  int32_t netCtlGetInfo(int32_t code, SceNetCtlInfo* info) final;

  /* SceNet facility */

  /* Resolver sub-facility */
  SceNetId resolverCreate(const char* name, int memid, int flags) final;
  int32_t resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) final;
  int32_t resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) final;
  int32_t resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) final;
  int32_t resolverGetError(SceNetId rid, int* result) final;
  int32_t resolverDestroy(SceNetId rid) final;
  int32_t resolverAbort(SceNetId rid, int flags) final;

  /* Epoll sub-facility */
  SceNetId epollCreate(const char* name, int flags) final;
  int32_t epollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) final;
  int32_t epollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) final;
  int32_t epollDestroy(SceNetId eid) final;
  int32_t epollAbort(SceNetId eid, int flags) final;

  /* Socket sub-facility */
  // guess what? todo.
};
