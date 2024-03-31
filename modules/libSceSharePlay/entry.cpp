#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceSharePlay);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSharePlay";

EXPORT SYSV_ABI int32_t sceSharePlayInitialize(void* pHeap, size_t heapSize) {
  LOG_USE_MODULE(libSceSharePlay);

  LOG_INFO(L"pHeap:0x%08llx heapSize:0x%08x", (uint64_t)pHeap, heapSize);

  LOG_ERR(L"TODO: %S", __FUNCTION__);

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSharePlayTerminate() {
  LOG_USE_MODULE(libSceSharePlay);

  LOG_ERR(L"TODO: %S", __FUNCTION__);

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSharePlaySetProhibition(SceSharePlayProhibitionMode mode) {
  LOG_USE_MODULE(libSceSharePlay);

  LOG_ERR(L"TODO: %S", __FUNCTION__);

  return Ok;
}

EXPORT SYSV_ABI int32_t sceSharePlayGetCurrentConnectionInfoA(SceSharePlayConnectionInfoA* pInfo) {
  LOG_USE_MODULE(libSceSharePlay);

  // *pInfo = {};

  LOG_ERR(L"TODO: %S", __FUNCTION__);

  return Ok;
}
}