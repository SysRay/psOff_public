#pragma once

#include "modules/external/libSceHttp/httpsTypes.h"
#include "modules/external/libSceHttp/types.h"
#include "modules/external/libSceNet/resolverTypes.h"
#include "modules/external/libSceNet/socketTypes.h"
#include "modules/external/libSceNet/types.h"
#include "modules/external/libSceNetCtl/types.h"
#include "modules/external/libSceSsl/types.h"
#include "modules_include/common.h"
#include "utility/utility.h"

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

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
  __APICALL static int32_t getLastError();

  /**
   * @brief returns errno ptr
   *
   * @return int32_t*
   */
  __APICALL static int32_t* getErrnoPtr();

  /* SceNetCtl facility*/

  /**
   * @brief returns information about specified network parameter
   *
   * @param code
   * @param info
   * @return int32_t
   */
  virtual int32_t netCtlGetInfo(int32_t code, SceNetCtlInfo* info) = 0;
  virtual int32_t netCtlGetState(int32_t* state)                   = 0;

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

  /* HTTP facility */

  /* HTTP1 facility */
  virtual int httpInit(int libnetMemId, int libsslCtxId, size_t poolSize)                                                                    = 0;
  virtual int httpTerm(int libhttpCtxId)                                                                                                     = 0;
  virtual int httpGetMemoryPoolStats(int libhttpCtxId, SceHttpMemoryPoolStats* currentStat)                                                  = 0;
  virtual int httpCreateTemplate(int libhttpCtxId, const char* userAgent, int httpVer, int isAutoProxyConf)                                  = 0;
  virtual int httpDeleteTemplate(int tmplId)                                                                                                 = 0;
  virtual int httpCreateConnection(int tmplId, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive)             = 0;
  virtual int httpCreateConnectionWithURL(int tmplId, const char* url, int isEnableKeepalive)                                                = 0;
  virtual int httpDeleteConnection(int connId)                                                                                               = 0;
  virtual int httpCreateRequest(int connId, int method, const char* path, uint64_t contentLength)                                            = 0;
  virtual int httpCreateRequest2(int connId, const char* method, const char* path, uint64_t contentLength)                                   = 0;
  virtual int httpCreateRequestWithURL(int connId, int method, const char* url, uint64_t contentLength)                                      = 0;
  virtual int httpCreateRequestWithURL2(int connId, const char* method, const char* url, uint64_t contentLength)                             = 0;
  virtual int httpDeleteRequest(int reqId)                                                                                                   = 0;
  virtual int httpSetRequestContentLength(int id, uint64_t contentLength)                                                                    = 0;
  virtual int httpSetChunkedTransferEnabled(int id, int isEnable)                                                                            = 0;
  virtual int httpSetInflateGZIPEnabled(int id, int isEnable)                                                                                = 0;
  virtual int httpSendRequest(int reqId, const void* postData, size_t size)                                                                  = 0;
  virtual int httpAbortRequest(int reqId)                                                                                                    = 0;
  virtual int httpGetResponseContentLength(int reqId, int* result, uint64_t* contentLength)                                                  = 0;
  virtual int httpGetStatusCode(int reqId, int* statusCode)                                                                                  = 0;
  virtual int httpGetAllResponseHeaders(int reqId, char** header, size_t* headerSize)                                                        = 0;
  virtual int httpReadData(int reqId, void* data, size_t size)                                                                               = 0;
  virtual int httpAddRequestHeader(int id, const char* name, const char* value, uint32_t mode)                                               = 0;
  virtual int httpRemoveRequestHeader(int id, const char* name)                                                                              = 0;
  virtual int httpParseResponseHeader(const char* header, size_t headerLen, const char* fieldStr, const char** fieldValue, size_t* valueLen) = 0;
  virtual int httpParseStatusLine(const char* statusLine, size_t lineLen, int* httpMajorVer, int* httpMinorVer, int* responseCode, const char** reasonPhrase,
                                  size_t* phraseLen)                                                                                         = 0;
  virtual int httpSetResponseHeaderMaxSize(int id, size_t headerSize)                                                                        = 0;
  virtual int httpSetAuthInfoCallback(int id, SceHttpAuthInfoCallback cbfunc, void* userArg)                                                 = 0;
  virtual int httpSetAuthEnabled(int id, int isEnable)                                                                                       = 0;
  virtual int httpGetAuthEnabled(int id, int* isEnable)                                                                                      = 0;
  virtual int httpAuthCacheFlush(int libhttpCtxId)                                                                                           = 0;
  virtual int httpSetRedirectCallback(int id, SceHttpRedirectCallback cbfunc, void* userArg)                                                 = 0;
  virtual int httpSetAutoRedirect(int id, int isEnable)                                                                                      = 0;
  virtual int httpGetAutoRedirect(int id, int* isEnable)                                                                                     = 0;
  virtual int httpRedirectCacheFlush(int libhttpCtxId)                                                                                       = 0;
  virtual int httpSetResolveTimeOut(int id, uint32_t usec)                                                                                   = 0;
  virtual int httpSetResolveRetry(int id, int retry)                                                                                         = 0;
  virtual int httpSetConnectTimeOut(int id, uint32_t usec)                                                                                   = 0;
  virtual int httpSetSendTimeOut(int id, uint32_t usec)                                                                                      = 0;
  virtual int httpSetRecvTimeOut(int id, uint32_t usec)                                                                                      = 0;
  virtual int httpSetRequestStatusCallback(int id, SceHttpRequestStatusCallback cbfunc, void* userArg)                                       = 0;
  virtual int httpGetLastErrno(int reqId, int* errNum)                                                                                       = 0;
  virtual int httpSetNonblock(int id, int isEnable)                                                                                          = 0;
  virtual int httpGetNonblock(int id, int* isEnable)                                                                                         = 0;
  virtual int httpTrySetNonblock(int id, int isEnable)                                                                                       = 0;
  virtual int httpTryGetNonblock(int id, int* isEnable)                                                                                      = 0;
  virtual int httpCreateEpoll(int libhttpCtxId, SceHttpEpollHandle* eh)                                                                      = 0;
  virtual int httpSetEpoll(int id, SceHttpEpollHandle eh, void* userArg)                                                                     = 0;
  virtual int httpUnsetEpoll(int id)                                                                                                         = 0;
  virtual int httpGetEpoll(int id, SceHttpEpollHandle* eh, void** userArg)                                                                   = 0;
  virtual int httpDestroyEpoll(int libhttpCtxId, SceHttpEpollHandle eh)                                                                      = 0;
  virtual int httpWaitRequest(SceHttpEpollHandle eh, SceHttpNBEvent* nbev, int maxevents, int timeout)                                       = 0;
  virtual int httpAbortWaitRequest(SceHttpEpollHandle eh)                                                                                    = 0;

  // HTTPS
  virtual int httpsLoadCert(int libhttpCtxId, int caCertNum, const SceSslData** caList, const SceSslData* cert, const SceSslData* privKey) = 0;
  virtual int httpsUnloadCert(int libhttpCtxId)                                                                                            = 0;
  virtual int httpsEnableOption(int id, uint32_t sslFlags)                                                                                 = 0;
  virtual int httpsDisableOption(int id, uint32_t sslFlags)                                                                                = 0;
  virtual int httpsGetSslError(int id, int* errNum, uint32_t* detail)                                                                      = 0;
  virtual int httpsSetSslCallback(int id, SceHttpsCallback cbfunc, void* userArg)                                                          = 0;
  virtual int httpsSetSslVersion(int id, SceSslVersion version)                                                                            = 0;
};

__APICALL INetworking& accessNetworking();
#undef __APICALL
