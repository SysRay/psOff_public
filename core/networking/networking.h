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

  protected:
  INetworking();

  public:
  ~INetworking();

  /**
   * @brief returns current network error code
   *
   * @return int32_t
   */
  static int32_t getLastError();

  /**
   * @brief returns errno ptr
   *
   * @return int32_t*
   */
  static int32_t* getErrnoPtr();

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
  virtual SceNetId resolverCreate(const char* name, int memid, int flags)                                                                              = 0;
  virtual int32_t  resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags)                    = 0;
  virtual int32_t  resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags)             = 0;
  virtual int32_t  resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) = 0;
  virtual int32_t  resolverGetError(SceNetId rid, int* result)                                                                                         = 0;
  virtual int32_t  resolverDestroy(SceNetId rid)                                                                                                       = 0;
  virtual int32_t  resolverAbort(SceNetId rid, int flags)                                                                                              = 0;

  /* Epoll sub-facility */
  virtual SceNetId epollCreate(const char* name, int flags)                                      = 0;
  virtual int      epollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event)      = 0;
  virtual int      epollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) = 0;
  virtual int      epollDestroy(SceNetId eid)                                                    = 0;
  virtual int      epollAbort(SceNetId eid, int flags)                                           = 0;

  /* Socket sub-facility */
  virtual SceNetId socketCreate(const char* name, int family, int type, int protocol)                                                = 0;
  virtual SceNetId socketAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen)                                          = 0;
  virtual int      socketBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen)                                       = 0;
  virtual int      socketConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen)                                    = 0;
  virtual int      socketGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen)                                     = 0;
  virtual int      socketGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen)                                     = 0;
  virtual int      socketGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen)                       = 0;
  virtual int      socketListen(SceNetId s, int backlog)                                                                             = 0;
  virtual int      socketRecv(SceNetId s, void* buf, size_t len, int flags)                                                          = 0;
  virtual int      socketRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen)      = 0;
  virtual int      socketRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags)                                                           = 0;
  virtual int      socketSend(SceNetId s, const void* msg, size_t len, int flags)                                                    = 0;
  virtual int      socketSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) = 0;
  virtual int      socketSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags)                                                     = 0;
  virtual int      socketSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen)                  = 0;
  virtual int      socketShutdown(SceNetId s, int how)                                                                               = 0;
  virtual int      socketClose(SceNetId s)                                                                                           = 0;
  virtual int      socketAbort(SceNetId s, int flags)                                                                                = 0;
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
