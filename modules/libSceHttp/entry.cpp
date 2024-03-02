#include "../libSceSsl/types.h"
#include "common.h"
#include "httpsTypes.h"
#include "logging.h"
#include "types.h"
#include "utility/utility.h"

LOG_DEFINE_MODULE(libSceHttp);

namespace {
using SceHttpsCallback = SYSV_ABI int (*)(int libsslCtxId, unsigned int verifyErr, SceSslCert* const sslCert[], int certNum, void* userArg);

} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceHttp";

EXPORT SYSV_ABI int sceHttpInit(int libnetMemId, int libsslCtxId, size_t poolSize) {
  static int id = 0;
  return ++id;
}

EXPORT SYSV_ABI int sceHttpTerm(int libhttpCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetMemoryPoolStats(int libhttpCtxId, SceHttpMemoryPoolStats* currentStat) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateTemplate(int libhttpCtxId, const char* userAgent, int httpVer, int isAutoProxyConf) {
  static int id = 0;
  return ++id;
}

EXPORT SYSV_ABI int sceHttpDeleteTemplate(int tmplId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateConnection(int tmplId, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateConnectionWithURL(int tmplId, const char* url, int isEnableKeepalive) {
  static int id = 0;
  // return ++id;
  return Err::CONNECT_TIMEOUT;
}

EXPORT SYSV_ABI int sceHttpDeleteConnection(int connId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateRequest(int connId, int method, const char* path, uint64_t contentLength) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateRequest2(int connId, const char* method, const char* path, uint64_t contentLength) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateRequestWithURL(int connId, int method, const char* url, uint64_t contentLength) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateRequestWithURL2(int connId, const char* method, const char* url, uint64_t contentLength) {
  return Err::SEND_TIMEOUT;
}

EXPORT SYSV_ABI int sceHttpDeleteRequest(int reqId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetRequestContentLength(int id, uint64_t contentLength) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetChunkedTransferEnabled(int id, int isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetInflateGZIPEnabled(int id, int isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSendRequest(int reqId, const void* postData, size_t size) {
  return Err::SEND_TIMEOUT;
}

EXPORT SYSV_ABI int sceHttpAbortRequest(int reqId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetResponseContentLength(int reqId, int* result, uint64_t* contentLength) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetStatusCode(int reqId, int* statusCode) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetAllResponseHeaders(int reqId, char** header, size_t* headerSize) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpReadData(int reqId, void* data, size_t size) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpAddRequestHeader(int id, const char* name, const char* value, uint32_t mode) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpRemoveRequestHeader(int id, const char* name) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpParseResponseHeader(const char* header, size_t headerLen, const char* fieldStr, const char** fieldValue, size_t* valueLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpParseStatusLine(const char* statusLine, size_t lineLen, int* httpMajorVer, int* httpMinorVer, int* responseCode,
                                           const char** reasonPhrase, size_t* phraseLen) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetResponseHeaderMaxSize(int id, size_t headerSize) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetAuthInfoCallback(int id, SceHttpAuthInfoCallback cbfunc, void* userArg) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetAuthEnabled(int id, int isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetAuthEnabled(int id, int* isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpAuthCacheFlush(int libhttpCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetRedirectCallback(int id, SceHttpRedirectCallback cbfunc, void* userArg) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetAutoRedirect(int id, int isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetAutoRedirect(int id, int* isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpRedirectCacheFlush(int libhttpCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetResolveTimeOut(int id, uint32_t usec) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetResolveRetry(int id, int retry) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetConnectTimeOut(int id, uint32_t usec) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetSendTimeOut(int id, uint32_t usec) {
  return Err::SEND_TIMEOUT;
}

EXPORT SYSV_ABI int sceHttpSetRecvTimeOut(int id, uint32_t usec) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetRequestStatusCallback(int id, SceHttpRequestStatusCallback cbfunc, void* userArg) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetLastErrno(int reqId, int* errNum) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetNonblock(int id, int isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetNonblock(int id, int* isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpTrySetNonblock(int id, int isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpTryGetNonblock(int id, int* isEnable) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpCreateEpoll(int libhttpCtxId, SceHttpEpollHandle* eh) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpSetEpoll(int id, SceHttpEpollHandle eh, void* userArg) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpUnsetEpoll(int id) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpGetEpoll(int id, SceHttpEpollHandle* eh, void** userArg) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpDestroyEpoll(int libhttpCtxId, SceHttpEpollHandle eh) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpWaitRequest(SceHttpEpollHandle eh, SceHttpNBEvent* nbev, int maxevents, int timeout) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpAbortWaitRequest(SceHttpEpollHandle eh) {
  return Ok;
}

// HTTPS
EXPORT SYSV_ABI int sceHttpsLoadCert(int libhttpCtxId, int caCertNum, const SceSslData** caList, const SceSslData* cert, const SceSslData* privKey) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpsUnloadCert(int libhttpCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpsEnableOption(int id, uint32_t sslFlags) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpsDisableOption(int id, uint32_t sslFlags) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpsGetSslError(int id, int* errNum, uint32_t* detail) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpsSetSslCallback(int id, SceHttpsCallback cbfunc, void* userArg) {
  return Ok;
}

EXPORT SYSV_ABI int sceHttpsSetSslVersion(int id, SceSslVersion version) {
  return Ok;
}
}