#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceAudioIn);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAudioIn";

EXPORT SYSV_ABI int32_t sceAudioInOpen(int32_t userId, int32_t type, int32_t index, uint32_t len, uint32_t freq, uint32_t param) {
  return Err::PORT_FULL;
}

EXPORT SYSV_ABI int32_t sceAudioInClose(int32_t handle) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioInInput(int32_t handle, void* dest) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioInGetSilentState(int32_t handle) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudioInHqOpen(int32_t userId, int32_t type, int32_t index, uint32_t timeLen, uint32_t freq, uint32_t param) {
  return Ok;
}
}