#include "codes.h"
#include "common.h"
#include "core/kernel/filesystem.h"
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

EXPORT SYSV_ABI int32_t sceNgs2CalcWaveformBlock(SceNgs2WaveformFormat* wf, uint32_t samPos, uint32_t samCount, void* out) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

static int32_t ProcessWaveData(WaveformInfo* wi, SceNgs2WaveformFormat* wf) {
  /* todo: Deal with WAV/VAG files */
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformData(const void* ptr, size_t size, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  if (ptr == nullptr) {
    return Err::INVALID_WAVEFORM_DATA;
  }

  WaveformInfo wi {
      .type = WAVEFORM_DATA,
      .ud   = {.dataPtr = ptr},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformFile(const char* path, long offset, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  filesystem::SceOpen flags {
      .mode = filesystem::SceOpenMode::RDONLY,
  };

  WaveformInfo wi {
      .type   = WAVEFORM_DATA,
      .ud     = {.fileHandle = filesystem::open(path, flags, {})},
      .offset = offset,
  };

  if (wi.ud.fileHandle == 0) {
    return Err::INVALID_WAVEFORM_DATA;
  }

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2ParseWaveformUser(SceWaveformUserFunc* user, size_t size, SceNgs2WaveformFormat* wf) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);

  if (user == nullptr) {
    return Err::INVALID_WAVEFORM_DATA;
  }

  WaveformInfo wi {
      .type = WAVEFORM_USER,
      .ud   = {.userFunc = user},
      .size = size,
  };

  return ProcessWaveData(&wi, wf);
}

EXPORT SYSV_ABI int32_t sceNgs2RackCreateWithAllocator(SceNgs2Handle* sh, uint32_t, void*, SceNgs2BufferAllocator* alloc, SceNgs2Handle* outh) {
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

EXPORT SYSV_ABI int32_t sceNgs2SystemCreateWithAllocator(SceNgs2SystemOption* sysopt, SceNgs2BufferAllocator* alloc, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S(%p, %p, %p)", __FUNCTION__, sysopt, alloc, outh);
  if (alloc == nullptr || alloc->allocHandler == nullptr) return Err::INVALID_BUFFER_ALLOCATOR;
  if (outh == nullptr) return Err::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size != sizeof(SceNgs2SystemOption)) return Err::INVALID_OPTION_SIZE;
  // todo: dealloc if (*outh) != nullptr
  return (*outh = new SceNgs2Handle) != nullptr ? 0 : Err::FAIL;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemDestroy(SceNgs2Handle* sh, SceNgs2ContextBufferInfo* cbi) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemRender(SceNgs2Handle* sh, SceNgs2RenderBufferInfo* rbi, int32_t count) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_TRACE(L"todo %S", __FUNCTION__);
  if (sh == nullptr) return Err::INVALID_SYSTEM_HANDLE;
  if (rbi->bufferPtr == nullptr) return Err::INVALID_BUFFER_ADDRESS;
  if (rbi->bufferSize == 0) return Err::INVALID_BUFFER_SIZE;
  if (rbi->waveType >= MAX_TYPES) return Err::INVALID_WAVEFORM_TYPE;
  if (rbi->channelsCount > 8) return Err::INVALID_NUM_CHANNELS;

  for (int32_t i = 0; i < count; i++) {
    if (rbi[i].bufferPtr != nullptr) {
      std::memset(rbi[i].bufferPtr, 0, rbi[i].bufferSize);
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

EXPORT SYSV_ABI int32_t sceNgs2SystemCreate(SceNgs2SystemOption* sysopt, SceNgs2ContextBufferInfo* cbi, SceNgs2Handle** outh) {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  if (outh == nullptr) return Err::INVALID_OUT_ADDRESS;
  if (sysopt != nullptr && sysopt->size != sizeof(SceNgs2SystemOption)) return Err::INVALID_OPTION_SIZE;

  *outh = new SceNgs2Handle;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNgs2SystemQueryBufferSize() {
  LOG_USE_MODULE(libSceNgs2);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
