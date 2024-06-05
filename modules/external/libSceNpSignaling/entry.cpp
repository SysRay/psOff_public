#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpSignaling);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpSignaling";

EXPORT SYSV_ABI int32_t sceNpSignalingInitialize(uint64_t poolSize, int threadPriority, int cpuAffinityMask, uint64_t threadStackSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpSignalingCreateContext(const SceNpId* npId, SceNpSignalingHandler handler, void* arg, uint32_t* ctxId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpSignalingCreateContextA(SceUserServiceUserId userId, SceNpSignalingHandler handler, void* arg, uint32_t* ctxId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpSignalingGetLocalNetInfo(uint32_t ctxId, SceNpSignalingNetInfo* info) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpSignalingSetContextOption(uint32_t ctxId, int optname, int optval) {
  return Ok;
}
}
