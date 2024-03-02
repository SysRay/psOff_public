#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceRemoteplay);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceRemoteplay";

EXPORT SYSV_ABI int sceRemoteplayInitialize(void* pHeap, size_t heapSize) {
  LOG_USE_MODULE(libSceRemoteplay);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRemoteplayTerminate() {
  LOG_USE_MODULE(libSceRemoteplay);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRemoteplayProhibit() {
  LOG_USE_MODULE(libSceRemoteplay);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRemoteplayApprove() {
  LOG_USE_MODULE(libSceRemoteplay);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRemoteplayProhibitStreaming(SceRemoteplayStreamingProhibition mode) {
  LOG_USE_MODULE(libSceRemoteplay);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceRemoteplayGetConnectionStatus(SceUserServiceUserId userid, SceRemoteplayConnectionStatus* pStatus) {
  *pStatus = SceRemoteplayConnectionStatus::Disconnect;
  LOG_USE_MODULE(libSceRemoteplay);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}