#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceAudiodec);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAudiodec";

EXPORT SYSV_ABI int32_t sceAudiodecInitLibrary(uint32_t codecType) {
  LOG_USE_MODULE(libSceAudiodec);
  LOG_INFO(L"InitLibrary| codecType: %P", codecType);

  if (!(codecType >= Err::SCE_AUDIODEC_TYPE_AT9 && codecType <= Err::SCE_AUDIODEC_TYPE_M4AAC)) {
    return Err::SCE_AUDIODEC_ERROR_INVALID_TYPE;
  } else {
    return 0;
  }
}

EXPORT SYSV_ABI int32_t sceAudiodecClearContext(int32_t handle) {
  LOG_USE_MODULE(libSceAudiodec);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudiodecTermLibrary(uint32_t codecType) {
  LOG_USE_MODULE(libSceAudiodec);
  LOG_INFO(L"TermLibrary| codecType: %P", codecType);

  if (!(codecType >= Err::SCE_AUDIODEC_TYPE_AT9 && codecType <= Err::SCE_AUDIODEC_TYPE_M4AAC)) {
    return Err::SCE_AUDIODEC_ERROR_INVALID_TYPE;
  } else {
    return 0;
  }
}
}