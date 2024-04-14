#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceAudio3d);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAudio3d";

EXPORT SYSV_ABI int32_t sceAudio3dInitialize(int64_t) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dPortOpen(int32_t userId, const SceAudio3dOpenParameters* params, SceAudio3dPortId* id) {
  return Err::OUT_OF_RESOURCES;
}

EXPORT SYSV_ABI int32_t sceAudio3dPortClose() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dPortPush() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dObjectReserve() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dObjectSetAttributes() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dObjectUnreserve() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dBedWrite() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dGetDefaultOpenParameters() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dPortAdvance() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dPortFlush() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dPortGetQueueLevel() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dPortSetAttribute() {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudio3dTerminate() {
  return Ok;
}
}
