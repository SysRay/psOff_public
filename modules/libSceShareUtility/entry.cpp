#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceShareUtility);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceShareUtility";

EXPORT SYSV_ABI int32_t sceShareUtilityInitialize(uint64_t funcFlag, size_t heapSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityInitializeEx2(uint64_t funcFlag, size_t heapSize, int threadPriority, SceKernelCpumask affMask) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityOpenShareMenu(SceContentSearchContentId id, SceContentSearchMimeType type) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityAdvanceInitialize(uint64_t funcFlag, size_t heapSize, int threadPriority, SceKernelCpumask affMask) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityOpenShareMenuBroadcast() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityOpenShareMenuDefault() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityTerminate() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityAdvanceTerminate() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityAdvanceSetUploadContentData(const char* data, size_t size) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityAdvanceWakeUpShareMenu(SceContentSearchContentId id, SceContentSearchMimeType type) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityAdvanceWakeUpShareMenuBroadcast() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceShareUtilityAdvanceWakeUpShareMenuDefault() {
  return Ok;
}
}
