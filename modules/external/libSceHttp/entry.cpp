#include "../libSceSsl/types.h"
#include "common.h"
#include "internal/networking/networking.h"
#include "httpsTypes.h"
#include "logging.h"
#include "types.h"

extern "C" {

EXPORT const char* MODULE_NAME = "libSceHttp";

EXPORT SYSV_ABI int sceHttpInit(int libnetMemId, int libsslCtxId, size_t poolSize) {
  return accessNetworking().httpInit(libnetMemId, libsslCtxId, poolSize);
}

EXPORT SYSV_ABI int sceHttpTerm(int libhttpCtxId) {
  return accessNetworking().httpTerm(libhttpCtxId);
}

EXPORT SYSV_ABI int sceHttpGetMemoryPoolStats(int libhttpCtxId, SceHttpMemoryPoolStats* currentStat) {
  return accessNetworking().httpGetMemoryPoolStats(libhttpCtxId, currentStat);
}

EXPORT SYSV_ABI int sceHttpCreateTemplate(int libhttpCtxId, const char* userAgent, int httpVer, int isAutoProxyConf) {
  return accessNetworking().httpCreateTemplate(libhttpCtxId, userAgent, httpVer, isAutoProxyConf);
}

EXPORT SYSV_ABI int sceHttpDeleteTemplate(int tmplId) {
  return accessNetworking().httpDeleteTemplate(tmplId);
}

EXPORT SYSV_ABI int sceHttpCreateConnection(int tmplId, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive) {
  return accessNetworking().httpCreateConnection(tmplId, serverName, scheme, port, isEnableKeepalive);
}

EXPORT SYSV_ABI int sceHttpCreateConnectionWithURL(int tmplId, const char* url, int isEnableKeepalive) {
  return accessNetworking().httpCreateConnectionWithURL(tmplId, url, isEnableKeepalive);
}

EXPORT SYSV_ABI int sceHttpDeleteConnection(int connId) {
  return accessNetworking().httpDeleteConnection(connId);
}

EXPORT SYSV_ABI int sceHttpCreateRequest(int connId, int method, const char* path, uint64_t contentLength) {
  return accessNetworking().httpCreateRequest(connId, method, path, contentLength);
}

EXPORT SYSV_ABI int sceHttpCreateRequest2(int connId, const char* method, const char* path, uint64_t contentLength) {
  return accessNetworking().httpCreateRequest2(connId, method, path, contentLength);
}

EXPORT SYSV_ABI int sceHttpCreateRequestWithURL(int connId, int method, const char* url, uint64_t contentLength) {
  return accessNetworking().httpCreateRequestWithURL(connId, method, url, contentLength);
}

EXPORT SYSV_ABI int sceHttpCreateRequestWithURL2(int connId, const char* method, const char* url, uint64_t contentLength) {
  return accessNetworking().httpCreateRequestWithURL2(connId, method, url, contentLength);
}

EXPORT SYSV_ABI int sceHttpDeleteRequest(int reqId) {
  return accessNetworking().httpDeleteRequest(reqId);
}

EXPORT SYSV_ABI int sceHttpSetRequestContentLength(int id, uint64_t contentLength) {
  return accessNetworking().httpSetRequestContentLength(id, contentLength);
}

EXPORT SYSV_ABI int sceHttpSetChunkedTransferEnabled(int id, int isEnable) {
  return accessNetworking().httpSetChunkedTransferEnabled(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpSetInflateGZIPEnabled(int id, int isEnable) {
  return accessNetworking().httpSetInflateGZIPEnabled(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpSendRequest(int reqId, const void* postData, size_t size) {
  return accessNetworking().httpSendRequest(reqId, postData, size);
}

EXPORT SYSV_ABI int sceHttpAbortRequest(int reqId) {
  return accessNetworking().httpAbortRequest(reqId);
}

EXPORT SYSV_ABI int sceHttpGetResponseContentLength(int reqId, int* result, uint64_t* contentLength) {
  return accessNetworking().httpGetResponseContentLength(reqId, result, contentLength);
}

EXPORT SYSV_ABI int sceHttpGetStatusCode(int reqId, int* statusCode) {
  return accessNetworking().httpGetStatusCode(reqId, statusCode);
}

EXPORT SYSV_ABI int sceHttpGetAllResponseHeaders(int reqId, char** header, size_t* headerSize) {
  return accessNetworking().httpGetAllResponseHeaders(reqId, header, headerSize);
}

EXPORT SYSV_ABI int sceHttpReadData(int reqId, void* data, size_t size) {
  return accessNetworking().httpReadData(reqId, data, size);
}

EXPORT SYSV_ABI int sceHttpAddRequestHeader(int id, const char* name, const char* value, uint32_t mode) {
  return accessNetworking().httpAddRequestHeader(id, name, value, mode);
}

EXPORT SYSV_ABI int sceHttpRemoveRequestHeader(int id, const char* name) {
  return accessNetworking().httpRemoveRequestHeader(id, name);
}

EXPORT SYSV_ABI int sceHttpParseResponseHeader(const char* header, size_t headerLen, const char* fieldStr, const char** fieldValue, size_t* valueLen) {
  return accessNetworking().httpParseResponseHeader(header, headerLen, fieldStr, fieldValue, valueLen);
}

EXPORT SYSV_ABI int sceHttpParseStatusLine(const char* statusLine, size_t lineLen, int* httpMajorVer, int* httpMinorVer, int* responseCode,
                                           const char** reasonPhrase, size_t* phraseLen) {
  return accessNetworking().httpParseStatusLine(statusLine, lineLen, httpMajorVer, httpMinorVer, responseCode, reasonPhrase, phraseLen);
}

EXPORT SYSV_ABI int sceHttpSetResponseHeaderMaxSize(int id, size_t headerSize) {
  return accessNetworking().httpSetResponseHeaderMaxSize(id, headerSize);
}

EXPORT SYSV_ABI int sceHttpSetAuthInfoCallback(int id, SceHttpAuthInfoCallback cbfunc, void* userArg) {
  return accessNetworking().httpSetAuthInfoCallback(id, cbfunc, userArg);
}

EXPORT SYSV_ABI int sceHttpSetAuthEnabled(int id, int isEnable) {
  return accessNetworking().httpSetAuthEnabled(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpGetAuthEnabled(int id, int* isEnable) {
  return accessNetworking().httpGetAuthEnabled(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpAuthCacheFlush(int libhttpCtxId) {
  return accessNetworking().httpAuthCacheFlush(libhttpCtxId);
}

EXPORT SYSV_ABI int sceHttpSetRedirectCallback(int id, SceHttpRedirectCallback cbfunc, void* userArg) {
  return accessNetworking().httpSetRedirectCallback(id, cbfunc, userArg);
}

EXPORT SYSV_ABI int sceHttpSetAutoRedirect(int id, int isEnable) {
  return accessNetworking().httpSetAutoRedirect(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpGetAutoRedirect(int id, int* isEnable) {
  return accessNetworking().httpGetAutoRedirect(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpRedirectCacheFlush(int libhttpCtxId) {
  return accessNetworking().httpRedirectCacheFlush(libhttpCtxId);
}

EXPORT SYSV_ABI int sceHttpSetResolveTimeOut(int id, uint32_t usec) {
  return accessNetworking().httpSetResolveTimeOut(id, usec);
}

EXPORT SYSV_ABI int sceHttpSetResolveRetry(int id, int retry) {
  return accessNetworking().httpSetResolveRetry(id, retry);
}

EXPORT SYSV_ABI int sceHttpSetConnectTimeOut(int id, uint32_t usec) {
  return accessNetworking().httpSetConnectTimeOut(id, usec);
}

EXPORT SYSV_ABI int sceHttpSetSendTimeOut(int id, uint32_t usec) {
  return accessNetworking().httpSetSendTimeOut(id, usec);
}

EXPORT SYSV_ABI int sceHttpSetRecvTimeOut(int id, uint32_t usec) {
  return accessNetworking().httpSetRecvTimeOut(id, usec);
}

EXPORT SYSV_ABI int sceHttpSetRequestStatusCallback(int id, SceHttpRequestStatusCallback cbfunc, void* userArg) {
  return accessNetworking().httpSetRequestStatusCallback(id, cbfunc, userArg);
}

EXPORT SYSV_ABI int sceHttpGetLastErrno(int reqId, int* errNum) {
  return accessNetworking().httpGetLastErrno(reqId, errNum);
}

EXPORT SYSV_ABI int sceHttpSetNonblock(int id, int isEnable) {
  return accessNetworking().httpSetNonblock(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpGetNonblock(int id, int* isEnable) {
  return accessNetworking().httpGetNonblock(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpTrySetNonblock(int id, int isEnable) {
  return accessNetworking().httpTrySetNonblock(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpTryGetNonblock(int id, int* isEnable) {
  return accessNetworking().httpTryGetNonblock(id, isEnable);
}

EXPORT SYSV_ABI int sceHttpCreateEpoll(int libhttpCtxId, SceHttpEpollHandle* eh) {
  return accessNetworking().httpCreateEpoll(libhttpCtxId, eh);
}

EXPORT SYSV_ABI int sceHttpSetEpoll(int id, SceHttpEpollHandle eh, void* userArg) {
  return accessNetworking().httpSetEpoll(id, eh, userArg);
}

EXPORT SYSV_ABI int sceHttpUnsetEpoll(int id) {
  return accessNetworking().httpUnsetEpoll(id);
}

EXPORT SYSV_ABI int sceHttpGetEpoll(int id, SceHttpEpollHandle* eh, void** userArg) {
  return accessNetworking().httpGetEpoll(id, eh, userArg);
}

EXPORT SYSV_ABI int sceHttpDestroyEpoll(int libhttpCtxId, SceHttpEpollHandle eh) {
  return accessNetworking().httpDestroyEpoll(libhttpCtxId, eh);
}

EXPORT SYSV_ABI int sceHttpWaitRequest(SceHttpEpollHandle eh, SceHttpNBEvent* nbev, int maxevents, int timeout) {
  return accessNetworking().httpWaitRequest(eh, nbev, maxevents, timeout);
}

EXPORT SYSV_ABI int sceHttpAbortWaitRequest(SceHttpEpollHandle eh) {
  return accessNetworking().httpAbortWaitRequest(eh);
}

// HTTPS
EXPORT SYSV_ABI int sceHttpsLoadCert(int libhttpCtxId, int caCertNum, const SceSslData** caList, const SceSslData* cert, const SceSslData* privKey) {
  return accessNetworking().httpsLoadCert(libhttpCtxId, caCertNum, caList, cert, privKey);
}

EXPORT SYSV_ABI int sceHttpsUnloadCert(int libhttpCtxId) {
  return accessNetworking().httpsUnloadCert(libhttpCtxId);
}

EXPORT SYSV_ABI int sceHttpsEnableOption(int id, uint32_t sslFlags) {
  return accessNetworking().httpsEnableOption(id, sslFlags);
}

EXPORT SYSV_ABI int sceHttpsDisableOption(int id, uint32_t sslFlags) {
  return accessNetworking().httpsDisableOption(id, sslFlags);
}

EXPORT SYSV_ABI int sceHttpsGetSslError(int id, int* errNum, uint32_t* detail) {
  return accessNetworking().httpsGetSslError(id, errNum, detail);
}

EXPORT SYSV_ABI int sceHttpsSetSslCallback(int id, SceHttpsCallback cbfunc, void* userArg) {
  return accessNetworking().httpsSetSslCallback(id, cbfunc, userArg);
}

EXPORT SYSV_ABI int sceHttpsSetSslVersion(int id, SceSslVersion version) {
  return accessNetworking().httpsSetSslVersion(id, version);
}

EXPORT SYSV_ABI int sceHttpUriEscape(char* out, size_t* req, size_t outsz, const char* str) {
  *req = 0;
  return 0;
}
}
