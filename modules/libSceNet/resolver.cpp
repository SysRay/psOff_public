#include "common.h"
#include "logging.h"
#include "resolverTypes.h"

LOG_DEFINE_MODULE(libSceNet);

namespace {} // namespace

extern "C" {
EXPORT SYSV_ABI SceNetId sceNetResolverCreate(const char* name, int memid, int flags) {
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);
  static size_t count = 0;
  return ++count;
}

EXPORT SYSV_ABI int sceNetResolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retry, int flags) {
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return getErr(ErrCode::_ETIMEDOUT);
}

EXPORT SYSV_ABI int sceNetResolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) {
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return getErr(ErrCode::_ETIMEDOUT);
}

EXPORT SYSV_ABI int sceNetResolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retry, int flags) {
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return getErr(ErrCode::_ETIMEDOUT);
}

EXPORT SYSV_ABI int sceNetResolverGetError(SceNetId rid, int* result) {
  LOG_USE_MODULE(libSceNet);
  LOG_ERR(L"todo %S", __FUNCTION__);
  *result = (int)ErrCode::_ETIMEDOUT;
  return Ok;
}

EXPORT SYSV_ABI int sceNetResolverDestroy(SceNetId rid) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetResolverAbort(SceNetId rid, int flags) {
  return Ok;
}
}
