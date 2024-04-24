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
  int32_t netCtlGetState(int32_t* state) final;

  /* SceNet facility */

  /* Resolver sub-facility */
  SceNetId resolverCreate(const char* name, int memid, int flags) final;
  int32_t  resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) final;
  int32_t  resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) final;
  int32_t  resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) final;
  int32_t  resolverGetError(SceNetId rid, int* result) final;
  int32_t  resolverDestroy(SceNetId rid) final;
  int32_t  resolverAbort(SceNetId rid, int flags) final;

  /* Epoll sub-facility */
  SceNetId epollCreate(const char* name, int flags) final;
  int32_t  epollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) final;
  int32_t  epollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) final;
  int32_t  epollDestroy(SceNetId eid) final;
  int32_t  epollAbort(SceNetId eid, int flags) final;

  /* Socket sub-facility */
  SceNetId socketCreate(const char* name, int family, int type, int protocol) final;
  SceNetId socketAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen) final;
  int      socketBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen) final;
  int      socketConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen) final;
  int      socketGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) final;
  int      socketGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) final;
  int      socketGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen) final;
  int      socketListen(SceNetId s, int backlog) final;
  int      socketRecv(SceNetId s, void* buf, size_t len, int flags) final;
  int      socketRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen) final;
  int      socketRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags) final;
  int      socketSend(SceNetId s, const void* msg, size_t len, int flags) final;
  int      socketSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) final;
  int      socketSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags) final;
  int      socketSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen) final;
  int      socketShutdown(SceNetId s, int how) final;
  int      socketClose(SceNetId s) final;
  int      socketAbort(SceNetId s, int flags) final;

  /* HTTP facility */

  /* HTTP1 facility */
  int httpInit(int libnetMemId, int libsslCtxId, size_t poolSize) final;
  int httpTerm(int libhttpCtxId) final;
  int httpGetMemoryPoolStats(int libhttpCtxId, SceHttpMemoryPoolStats* currentStat) final;
  int httpCreateTemplate(int libhttpCtxId, const char* userAgent, int httpVer, int isAutoProxyConf) final;
  int httpDeleteTemplate(int tmplId) final;
  int httpCreateConnection(int tmplId, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive) final;
  int httpCreateConnectionWithURL(int tmplId, const char* url, int isEnableKeepalive) final;
  int httpDeleteConnection(int connId) final;
  int httpCreateRequest(int connId, int method, const char* path, uint64_t contentLength) final;
  int httpCreateRequest2(int connId, const char* method, const char* path, uint64_t contentLength) final;
  int httpCreateRequestWithURL(int connId, int method, const char* url, uint64_t contentLength) final;
  int httpCreateRequestWithURL2(int connId, const char* method, const char* url, uint64_t contentLength) final;
  int httpDeleteRequest(int reqId) final;
  int httpSetRequestContentLength(int id, uint64_t contentLength) final;
  int httpSetChunkedTransferEnabled(int id, int isEnable) final;
  int httpSetInflateGZIPEnabled(int id, int isEnable) final;
  int httpSendRequest(int reqId, const void* postData, size_t size) final;
  int httpAbortRequest(int reqId) final;
  int httpGetResponseContentLength(int reqId, int* result, uint64_t* contentLength) final;
  int httpGetStatusCode(int reqId, int* statusCode) final;
  int httpGetAllResponseHeaders(int reqId, char** header, size_t* headerSize) final;
  int httpReadData(int reqId, void* data, size_t size) final;
  int httpAddRequestHeader(int id, const char* name, const char* value, uint32_t mode) final;
  int httpRemoveRequestHeader(int id, const char* name) final;
  int httpParseResponseHeader(const char* header, size_t headerLen, const char* fieldStr, const char** fieldValue, size_t* valueLen) final;
  int httpParseStatusLine(const char* statusLine, size_t lineLen, int* httpMajorVer, int* httpMinorVer, int* responseCode, const char** reasonPhrase,
                          size_t* phraseLen) final;
  int httpSetResponseHeaderMaxSize(int id, size_t headerSize) final;
  int httpSetAuthInfoCallback(int id, SceHttpAuthInfoCallback cbfunc, void* userArg) final;
  int httpSetAuthEnabled(int id, int isEnable) final;
  int httpGetAuthEnabled(int id, int* isEnable) final;
  int httpAuthCacheFlush(int libhttpCtxId) final;
  int httpSetRedirectCallback(int id, SceHttpRedirectCallback cbfunc, void* userArg) final;
  int httpSetAutoRedirect(int id, int isEnable) final;
  int httpGetAutoRedirect(int id, int* isEnable) final;
  int httpRedirectCacheFlush(int libhttpCtxId) final;
  int httpSetResolveTimeOut(int id, uint32_t usec) final;
  int httpSetResolveRetry(int id, int retry) final;
  int httpSetConnectTimeOut(int id, uint32_t usec) final;
  int httpSetSendTimeOut(int id, uint32_t usec) final;
  int httpSetRecvTimeOut(int id, uint32_t usec) final;
  int httpSetRequestStatusCallback(int id, SceHttpRequestStatusCallback cbfunc, void* userArg) final;
  int httpGetLastErrno(int reqId, int* errNum) final;
  int httpSetNonblock(int id, int isEnable) final;
  int httpGetNonblock(int id, int* isEnable) final;
  int httpTrySetNonblock(int id, int isEnable) final;
  int httpTryGetNonblock(int id, int* isEnable) final;
  int httpCreateEpoll(int libhttpCtxId, SceHttpEpollHandle* eh) final;
  int httpSetEpoll(int id, SceHttpEpollHandle eh, void* userArg) final;
  int httpUnsetEpoll(int id) final;
  int httpGetEpoll(int id, SceHttpEpollHandle* eh, void** userArg) final;
  int httpDestroyEpoll(int libhttpCtxId, SceHttpEpollHandle eh) final;
  int httpWaitRequest(SceHttpEpollHandle eh, SceHttpNBEvent* nbev, int maxevents, int timeout) final;
  int httpAbortWaitRequest(SceHttpEpollHandle eh) final;

  // HTTPS
  int httpsLoadCert(int libhttpCtxId, int caCertNum, const SceSslData** caList, const SceSslData* cert, const SceSslData* privKey) final;
  int httpsUnloadCert(int libhttpCtxId) final;
  int httpsEnableOption(int id, uint32_t sslFlags) final;
  int httpsDisableOption(int id, uint32_t sslFlags) final;
  int httpsGetSslError(int id, int* errNum, uint32_t* detail) final;
  int httpsSetSslCallback(int id, SceHttpsCallback cbfunc, void* userArg) final;
  int httpsSetSslVersion(int id, SceSslVersion version) final;
};
