#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libScePadTracker);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePadTracker";

EXPORT SYSV_ABI int32_t scePadTrackerCalibrate(int32_t* onionSize, int32_t* garlicSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePadTrackerGetWorkingMemorySize(int32_t* onionSize, int32_t* garlicSize) {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePadTrackerInit(void* onionMemory, void* garlicMemory, int pipeId, int queueId) {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePadTrackerReadState(int32_t handle, ScePadTrackerData* data) {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePadTrackerTerm() {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePadTrackerUpdate(ScePadTrackerInput input) {
  return Ok;
}
}