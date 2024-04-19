#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceDepth);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceDepth";

EXPORT SYSV_ABI int sceDepthInitialize(const SceDepthInitializeParameter* initializeParameter,
	                                     const SceDepthMemoryInformation* memoryInformation) {
  LOG_USE_MODULE(libSceDepth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceDepthQueryMemory(SceDepthInitializeParameter* initializeParameter,
	                                      SceDepthQueryMemoryResult* queryMemoryResult) {
  LOG_USE_MODULE(libSceDepth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceDepthEnableExtendedMode(uint32_t param1) {
  LOG_USE_MODULE(libSceDepth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int sceDepthTerminate(int handle) {
  LOG_USE_MODULE(libSceDepth);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}