#include "offline.h"

#include "logging.h"
#include "modules/libSceNetCtl/codes.h"

LOG_DEFINE_MODULE(OfflineNetworkingCore)

int32_t OfflineNet::netCtlGetInfo(int32_t code, SceNetCtlInfo* info) {
  LOG_USE_MODULE(OfflineNetworkingCore);
  LOG_TRACE(L"netCtlGetInfo(%d, %p)", code, info);
  ::memset(info, 0, sizeof(SceNetCtlInfo));
  return Err::NetCtl::NOT_CONNECTED; // todo simulate available network connection?
}

int32_t OfflineNet::resolverCreate(const char* name, int memid, int flags) {
  return 0;
}

int32_t OfflineNet::resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) {
  return Ok;
}

int32_t OfflineNet::resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) {
  return Ok;
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
