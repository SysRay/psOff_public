#include "offline.h"

#include "logging.h"
#include "modules/external/libSceNetCtl/codes.h"

LOG_DEFINE_MODULE(OfflineNetworkingCore);

int32_t OfflineNet::netCtlGetInfo(int32_t code, SceNetCtlInfo* info) {
  LOG_USE_MODULE(OfflineNetworkingCore);
  LOG_TRACE(L"netCtlGetInfo(%d, %p)", code, info);

  switch (code) {
    case 1: info->device = 0; break;
    case 2: memset(info->ether_addr.data, 0, SCE_NET_ETHER_ADDR_LEN); break;
    case 3: info->mtu = 0; break;
    case 4: info->link = 0; break;
    case 5: memset(info->bssid.data, 0, SCE_NET_ETHER_ADDR_LEN); break;
    case 6: *info->ssid = '\0'; break;
    case 7: info->wifi_security = 0; break;
    case 8: info->rssi_dbm = 0; break;
    case 9: info->rssi_percentage = 0; break;
    case 10: info->channel = 0; break;
    case 11: info->ip_config = 0; break;
    case 12: *info->dhcp_hostname = '\0'; break;
    case 13: *info->pppoe_auth_name = '\0'; break;
    case 14: *info->ip_address = '\0'; break;
    case 15: *info->netmask = '\0'; break;
    case 16: *info->default_route = '\0'; break;
    case 17: *info->primary_dns = '\0'; break;
    case 18: *info->secondary_dns = '\0'; break;
    case 19: info->http_proxy_config = 0; break;
    case 20: *info->http_proxy_server = '\0'; break;
    case 21: info->http_proxy_port = 0; break;
  }

  return Err::NetCtl::NOT_CONNECTED;
}

int32_t OfflineNet::netCtlGetState(int32_t* state) {
  *state = 0; // Network disconnected
  return Ok;
}

SceNetId OfflineNet::resolverCreate(const char* name, int memid, int flags) {
  static SceNetId id = 0;
  return ++id;
}

int32_t OfflineNet::resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) {
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OfflineNet::resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) {
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OfflineNet::resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) {
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OfflineNet::resolverGetError(SceNetId rid, int* result) {
  *result = (int)ErrCode::_ETIMEDOUT;
  return Ok;
}

int32_t OfflineNet::resolverDestroy(SceNetId rid) {
  return Ok;
}

int32_t OfflineNet::resolverAbort(SceNetId rid, int flags) {
  return Ok;
}

SceNetId OfflineNet::epollCreate(const char* name, int flags) {
  static SceNetId id = 0;
  return ++id;
}

int32_t OfflineNet::epollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) {
  return Ok;
}

int32_t OfflineNet::epollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) {
  return Ok;
}

int32_t OfflineNet::epollDestroy(SceNetId eid) {
  return Ok;
}

int32_t OfflineNet::epollAbort(SceNetId eid, int flags) {
  return Ok;
}

SceNetId OfflineNet::socketCreate(const char* name, int family, int type, int protocol) {
  *INetworking::getErrnoPtr() = NetErrNo::SCE_NET_EPROTONOSUPPORT;
  return Err::Net::ERROR_EINVAL;
}

SceNetId OfflineNet::socketAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen) {
  return 0;
}

int OfflineNet::socketBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen) {
  return Ok;
}

int OfflineNet::socketConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen) {
  return Ok;
}

int OfflineNet::socketGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return Ok;
}

int OfflineNet::socketGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return Ok;
}

int OfflineNet::socketGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen) {
  return 0;
}

int OfflineNet::socketListen(SceNetId s, int backlog) {
  return Ok;
}

int OfflineNet::socketRecv(SceNetId s, void* buf, size_t len, int flags) {
  return 0;
}

int OfflineNet::socketRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen) {
  return 0;
}

int OfflineNet::socketRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags) {
  return Ok;
}

int OfflineNet::socketSend(SceNetId s, const void* msg, size_t len, int flags) {
  return 0;
}

int OfflineNet::socketSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) {
  return 0;
}

int OfflineNet::socketSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags) {
  return Ok;
}

int OfflineNet::socketSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen) {
  return Ok;
}

int OfflineNet::socketShutdown(SceNetId s, int how) {
  return Ok;
}

int OfflineNet::socketClose(SceNetId s) {
  return Ok;
}

int OfflineNet::socketAbort(SceNetId s, int flags) {
  return Ok;
}

int OfflineNet::httpInit(int libnetMemId, int libsslCtxId, size_t poolSize) {
  return Ok;
}

int OfflineNet::httpTerm(int libhttpCtxId) {
  return Ok;
}

int OfflineNet::httpGetMemoryPoolStats(int libhttpCtxId, SceHttpMemoryPoolStats* currentStat) {
  return Ok;
}

int OfflineNet::httpCreateTemplate(int libhttpCtxId, const char* userAgent, int httpVer, int isAutoProxyConf) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpDeleteTemplate(int tmplId) {
  return Ok;
}

int OfflineNet::httpCreateConnection(int tmplId, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpCreateConnectionWithURL(int tmplId, const char* url, int isEnableKeepalive) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpDeleteConnection(int connId) {
  return Ok;
}

int OfflineNet::httpCreateRequest(int connId, int method, const char* path, uint64_t contentLength) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpCreateRequest2(int connId, const char* method, const char* path, uint64_t contentLength) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpCreateRequestWithURL(int connId, int method, const char* url, uint64_t contentLength) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpCreateRequestWithURL2(int connId, const char* method, const char* url, uint64_t contentLength) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpDeleteRequest(int reqId) {
  return Ok;
}

int OfflineNet::httpSetRequestContentLength(int id, uint64_t contentLength) {
  return Ok;
}

int OfflineNet::httpSetChunkedTransferEnabled(int id, int isEnable) {
  return Ok;
}

int OfflineNet::httpSetInflateGZIPEnabled(int id, int isEnable) {
  return Ok;
}

int OfflineNet::httpSendRequest(int reqId, const void* postData, size_t size) {
  return Ok;
}

int OfflineNet::httpAbortRequest(int reqId) {
  return Ok;
}

int OfflineNet::httpGetResponseContentLength(int reqId, int* result, uint64_t* contentLength) {
  return Ok;
}

int OfflineNet::httpGetStatusCode(int reqId, int* statusCode) {
  return Ok;
}

int OfflineNet::httpGetAllResponseHeaders(int reqId, char** header, size_t* headerSize) {
  return Ok;
}

int OfflineNet::httpReadData(int reqId, void* data, size_t size) {
  return Ok;
}

int OfflineNet::httpAddRequestHeader(int id, const char* name, const char* value, uint32_t mode) {
  return Ok;
}

int OfflineNet::httpRemoveRequestHeader(int id, const char* name) {
  return Ok;
}

int OfflineNet::httpParseResponseHeader(const char* header, size_t headerLen, const char* fieldStr, const char** fieldValue, size_t* valueLen) {
  return Ok;
}

int OfflineNet::httpParseStatusLine(const char* statusLine, size_t lineLen, int* httpMajorVer, int* httpMinorVer, int* responseCode, const char** reasonPhrase,
                                    size_t* phraseLen) {
  return Ok;
}

int OfflineNet::httpSetResponseHeaderMaxSize(int id, size_t headerSize) {
  return Ok;
}

int OfflineNet::httpSetAuthInfoCallback(int id, SceHttpAuthInfoCallback cbfunc, void* userArg) {
  return Ok;
}

int OfflineNet::httpSetAuthEnabled(int id, int isEnable) {
  return Ok;
}

int OfflineNet::httpGetAuthEnabled(int id, int* isEnable) {
  return Ok;
}

int OfflineNet::httpAuthCacheFlush(int libhttpCtxId) {
  return Ok;
}

int OfflineNet::httpSetRedirectCallback(int id, SceHttpRedirectCallback cbfunc, void* userArg) {
  return Ok;
}

int OfflineNet::httpSetAutoRedirect(int id, int isEnable) {
  return Ok;
}

int OfflineNet::httpGetAutoRedirect(int id, int* isEnable) {
  return Ok;
}

int OfflineNet::httpRedirectCacheFlush(int libhttpCtxId) {
  return Ok;
}

int OfflineNet::httpSetResolveTimeOut(int id, uint32_t usec) {
  return Ok;
}

int OfflineNet::httpSetResolveRetry(int id, int retry) {
  return Ok;
}

int OfflineNet::httpSetConnectTimeOut(int id, uint32_t usec) {
  return Ok;
}

int OfflineNet::httpSetSendTimeOut(int id, uint32_t usec) {
  return Ok;
}

int OfflineNet::httpSetRecvTimeOut(int id, uint32_t usec) {
  return Ok;
}

int OfflineNet::httpSetRequestStatusCallback(int id, SceHttpRequestStatusCallback cbfunc, void* userArg) {
  return Ok;
}

int OfflineNet::httpGetLastErrno(int reqId, int* errNum) {
  return Ok;
}

int OfflineNet::httpSetNonblock(int id, int isEnable) {
  return Ok;
}

int OfflineNet::httpGetNonblock(int id, int* isEnable) {
  return Ok;
}

int OfflineNet::httpTrySetNonblock(int id, int isEnable) {
  return Ok;
}

int OfflineNet::httpTryGetNonblock(int id, int* isEnable) {
  return Ok;
}

int OfflineNet::httpCreateEpoll(int libhttpCtxId, SceHttpEpollHandle* eh) {
  static int id = 0;
  return ++id;
}

int OfflineNet::httpSetEpoll(int id, SceHttpEpollHandle eh, void* userArg) {
  return Ok;
}

int OfflineNet::httpUnsetEpoll(int id) {
  return Ok;
}

int OfflineNet::httpGetEpoll(int id, SceHttpEpollHandle* eh, void** userArg) {
  return Ok;
}

int OfflineNet::httpDestroyEpoll(int libhttpCtxId, SceHttpEpollHandle eh) {
  return Ok;
}

int OfflineNet::httpWaitRequest(SceHttpEpollHandle eh, SceHttpNBEvent* nbev, int maxevents, int timeout) {
  return Ok;
}

int OfflineNet::httpAbortWaitRequest(SceHttpEpollHandle eh) {
  return Ok;
}

// HTTPS
int OfflineNet::httpsLoadCert(int libhttpCtxId, int caCertNum, const SceSslData** caList, const SceSslData* cert, const SceSslData* privKey) {
  return Ok;
}

int OfflineNet::httpsUnloadCert(int libhttpCtxId) {
  return Ok;
}

int OfflineNet::httpsEnableOption(int id, uint32_t sslFlags) {
  return Ok;
}

int OfflineNet::httpsDisableOption(int id, uint32_t sslFlags) {
  return Ok;
}

int OfflineNet::httpsGetSslError(int id, int* errNum, uint32_t* detail) {
  return Ok;
}

int OfflineNet::httpsSetSslCallback(int id, SceHttpsCallback cbfunc, void* userArg) {
  return Ok;
}

int OfflineNet::httpsSetSslVersion(int id, SceSslVersion version) {
  return Ok;
}
