#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceAjm);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAjm";

EXPORT SYSV_ABI int32_t sceAjmInitialize(int64_t iReserved, SceAjmContextId* const pContext) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmFinalize(const SceAjmContextId uiContext) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmMemoryRegister(const SceAjmContextId uiContext, void* const pRegion, const size_t szNumPages) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmMemoryUnregister(const SceAjmContextId uiContext, void* const pRegion) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmModuleRegister(const SceAjmContextId uiContext, const SceAjmCodecType uiCodec, int64_t iReserved) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmModuleUnregister(const SceAjmContextId uiContext, const SceAjmCodecType uiCodec) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmInstanceCodecType(SceAjmCodecType uiCodec) {
  return uiCodec >> 0xE;
}

EXPORT SYSV_ABI int32_t sceAjmInstanceCreate(const SceAjmContextId uiContext, const SceAjmCodecType uiCodec, const uint64_t uiFlags,
                                             SceAjmInstanceId* const pInstance) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmInstanceExtend(const SceAjmContextId uiContext, const SceAjmCodecType uiCodec, const uint64_t uiFlags,
                                             SceAjmInstanceId uiInstance) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmInstanceSwitch(const SceAjmContextId uiContext, const SceAjmCodecType uiCodec, const uint64_t uiFlags,
                                             const SceAjmInstanceId uiInstance) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmInstanceDestroy(const SceAjmContextId uiContext, const SceAjmInstanceId uiInstance) {
  return Ok;
}
}