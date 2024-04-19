#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceMoveTracker);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceMoveTracker";

EXPORT SYSV_ABI int32_t sceMoveTrackerInit(void* onionMemory,
                                           void* garlicMemory,
                                           int pipeId,
                                           int queueId) {
  LOG_USE_MODULE(libSceMoveTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceMoveTrackerGetWorkingMemorySize(int *onionSize,
                                                           int *garlicSize) {
  LOG_USE_MODULE(libSceMoveTracker);
   if (onionSize == nullptr || garlicSize == nullptr) {
    return Err::ERROR_INVALID_ARG;
   }
   *onionSize  = 0x800000 + 0x200000;
   *garlicSize = 0x800000;
   return Ok;
}

EXPORT SYSV_ABI int32_t sceMoveTrackerControllersUpdate(SceMoveTrackerControllerInput controllerInputs) {
  LOG_USE_MODULE(libSceMoveTracker);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}