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

void FixSideband(uint64_t uiFlags, void* pSidebandOutput, uint64_t szSidebandOutputSize) {
  if ((uiFlags & SCE_AJM_FLAG_SIDEBAND_STREAM) != 0) {
    SceAjmSidebandStreamResult* pResult    = reinterpret_cast<SceAjmSidebandStreamResult*>(pSidebandOutput);
    pResult->sStream.iSizeConsumed         = 1;
    pResult->sStream.iSizeProduced         = 1;
    pResult->sStream.uiTotalDecodedSamples = 1;
  }
}

EXPORT SYSV_ABI int32_t sceAjmBatchJobControlBufferRa(void* pBatchPosition, SceAjmInstanceId uiInstance, uint64_t uiFlags, void* pSidebandInput,
                                                      uint64_t szSidebandInputSize, void* pSidebandOutput, uint64_t szSidebandOutputSize,
                                                      void** pReturnAddress) {
  if (pSidebandOutput != nullptr) {
    std::memset(pSidebandOutput, 0, szSidebandOutputSize);
    FixSideband(uiFlags, pSidebandOutput, szSidebandOutputSize);
  }
  return 0;
}

EXPORT SYSV_ABI int32_t sceAjmBatchJobRunBufferRa(void* pBatchPosition, SceAjmInstanceId uiInstance, uint64_t uiFlags, void* pDataInput,
                                                  uint64_t szDataInputSize, void* pDataOutput, uint64_t szDataOutputSize, void* pSidebandOutput,
                                                  uint64_t szSidebandOutputSize, void** pReturnAddress) {
  if (pSidebandOutput != nullptr) {
    std::memset(pSidebandOutput, 0, szSidebandOutputSize);
    FixSideband(uiFlags, pSidebandOutput, szSidebandOutputSize);
  }

  std::memset(pDataOutput, 0, szDataOutputSize);
  return 0;
}

EXPORT SYSV_ABI int32_t sceAjmBatchStartBuffer(SceAjmContextId uiContext, void* pBatchCommands, uint64_t szBatchSize, int iPriority,
                                               SceAjmBatchError pBatchError, SceAjmBatchId pBatch) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAjmBatchWait(SceAjmContextId uiContext, SceAjmBatchId uiBatch, uint32_t uiTimeout, SceAjmBatchError pBatchError) {
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

EXPORT SYSV_ABI int32_t sceAjmInstanceCodecType(const SceAjmCodecType uiCodec) {
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