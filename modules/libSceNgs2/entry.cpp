#include "codes.h"
#include "common.h"
#include "core/kernel/pthread.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNgs2);

extern "C" {

enum WaveformInfoType {
  WAVEFORM_FILE,
  WAVEFORM_DATA,
  WAVEFORM_USER,
};

struct WaveformInfo {
  WaveformInfoType type;
  char             pad[4];

  union WaveformPtr {
    int                  fileHandle;
    SceWaveformUserFunc* userFunc;
    const void*          dataPtr;
  } ud;

  size_t size;
  long   offset;
};

EXPORT const char* MODULE_NAME = "libSceNgs2";

EXPORT SYSV_ABI int32_t sceNgs2CalcWaveformBlock(SceWaveformFormat* wf, uint32_t samPos, uint32_t samCount, void* out) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

static int32_t ProcessWaveData(WaveformInfo* wi, SceWaveformFormat* wf) {
  /* todo: Deal with WAV/VAG files */
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformData(const void* ptr, size_t size, SceWaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  WaveformInfo wi {
      .type = WAVEFORM_DATA,
      .ud   = {.dataPtr = ptr},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformFile(const char* path, long offset, SceWaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  WaveformInfo wi {
      .type   = WAVEFORM_DATA,
      .ud     = {.fileHandle = 0 /* libSceLibcInternal.prx::fopen(path, "rb") */},
      .offset = offset,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformUser(SceWaveformUserFunc* user, size_t size, SceWaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  if (user == nullptr) {
    return 0x804a0408 /* SCE_NGS2_ERROR_INVALID_WAVEFORM_DATA */;
  }

  WaveformInfo wi {
      .type = WAVEFORM_USER,
      .ud   = {.userFunc = user},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreateWithAllocator() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackDestroy(SceNgs2Handle* rh, void*) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackGetVoiceHandle() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreateWithAllocator(SceSystemOption* sysopt, SceNgs2BufferAllocator* alloc, SceNgs2Handle* sh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S(%p, %p, %p)", __FUNCTION__, sysopt, alloc, sh);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemDestroy(SceNgs2Handle* sh, SceNgs2ContextBufferInfo* cbi) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemRender(SceNgs2Handle* sh, SceNgs2RenderBufferInfo* param_2, int32_t count) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);

  for (int32_t i = 0; i < count; i++) {
    if (param_2[i].bufferPtr != nullptr) {
      std::memset(param_2[i].bufferPtr, 0, param_2[i].bufferSize);
    }
  }

  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemSetGrainSamples(SceNgs2Handle* sh, uint32_t samplesCount) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceControl() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetState() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2VoiceGetStateFlags() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomApply() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomCalcListener() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomResetListenerParam(SceNgs2GeomListenerParam* param) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2GeomResetSourceParam(SceNgs2GeomSourceParam* param) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreate() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2RackQueryBufferSize() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemCreate() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemQueryBufferSize() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
