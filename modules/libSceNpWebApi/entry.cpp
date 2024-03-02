#include "../libSceNpManager/types.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpWebApi);

namespace {
using SceNpWebApiExtdPushEventCallbackA = SYSV_ABI void (*)(int32_t userCtxId, int32_t callbackId, const char* pNpServiceName, SceNpServiceLabel npServiceLabel,
                                                            const SceNpPeerAddressA* pTo, const SceNpOnlineId* pToOnlineId, const SceNpPeerAddressA* pFrom,
                                                            const SceNpOnlineId* pFromOnlineId, const SceNpWebApiPushEventDataType* pDataType,
                                                            const char* pData, size_t dataLen, const SceNpWebApiExtdPushEventExtdData* pExtdData,
                                                            size_t extdDataNum, void* pUserArg);

} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpWebApi";

EXPORT SYSV_ABI int32_t sceNpWebApiCreateExtdPushEventFilter(int32_t libCtxId, int32_t handleId, const char* pNpServiceName, SceNpServiceLabel npServiceLabel,
                                                             const SceNpWebApiExtdPushEventFilterParameter* pFilterParam, size_t filterParamNum) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiDeleteExtdPushEventFilter(int32_t libCtxId, int32_t filterId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiRegisterExtdPushEventCallbackA(int32_t userCtxId, int32_t filterId, SceNpWebApiExtdPushEventCallbackA cbFunc,
                                                                  void* pUserArg) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiUnregisterExtdPushEventCallback(int32_t userCtxId, int32_t callbackId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiInitialize(int libHttpCtxId, size_t poolSize) {
  static int id = 0;
  return ++id;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiTerminate(int32_t libCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiCreateContextA(int32_t libCtxId, int32_t userId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiDeleteContext(int32_t titleUserCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiCreateRequest(int32_t titleUserCtxId, const char* pApiGroup, const char* pPath, SceNpWebApiHttpMethod method,
                                                 const SceNpWebApiContentParameter* pContentParameter, int64_t* pRequestId) {
  *pRequestId = 0;
  return -1;
}

EXPORT SYSV_ABI int32_t sceNpWebApiCreateMultipartRequest(int32_t titleUserCtxId, const char* pApiGroup, const char* pPath, SceNpWebApiHttpMethod method,
                                                          int64_t* pRequestId) {
  *pRequestId = 0;
  return -1;
}

EXPORT SYSV_ABI int32_t sceNpWebApiDeleteRequest(int64_t requestId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiSendRequest2(int64_t requestId, const void* pData, size_t dataSize, SceNpWebApiResponseInformationOption* pRespInfoOption) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiSendMultipartRequest2(int64_t requestId, int32_t partIndex, const void* pData, size_t dataSize,
                                                         SceNpWebApiResponseInformationOption* pRespInfoOption) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiReadData(int64_t requestId, void* pData, size_t size) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiSetMultipartContentType(int64_t requestId, const char* pTypeName, const char* pBoundary) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiAddMultipartPart(int64_t requestId, const SceNpWebApiMultipartPartParameter* pParam, int32_t* pIndex) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiAddHttpRequestHeader(int64_t requestId, const char* pFieldName, const char* pValue) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiGetHttpResponseHeaderValueLength(int64_t requestId, const char* pFieldName, size_t* pValueLength) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiGetHttpResponseHeaderValue(int64_t requestId, const char* pFieldName, char* pValue, size_t valueSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiAbortRequest(int64_t requestId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiCreateHandle(int32_t libCtxId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiDeleteHandle(int32_t libCtxId, int32_t handleId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiAbortHandle(int32_t libCtxId, int32_t handleId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiGetMemoryPoolStats(int32_t libCtxId, SceNpWebApiMemoryPoolStats* pCurrentStat) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiSetMaxConnection(int32_t libCtxId, int32_t maxConnection) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiClearUnusedConnection(int32_t userCtxId, const char* pApiGroup, bool bRemainKeepAliveConnection) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiClearAllUnusedConnection(int32_t userCtxId, bool bRemainKeepAliveConnection) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiGetConnectionStats(int32_t userCtxId, const char* pApiGroup, SceNpWebApiConnectionStats* pStats) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiSetRequestTimeout(int64_t requestId, uint32_t timeout) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpWebApiSetHandleTimeout(int32_t libCtxId, int32_t handleId, uint32_t timeout) {
  return Ok;
}

EXPORT SYSV_ABI void sceNpWebApiCheckTimeout(void) {}

EXPORT SYSV_ABI int32_t sceNpWebApiUtilityParseNpId(const char* pJsonNpId, SceNpId* pNpId) {
  return Ok;
}
}