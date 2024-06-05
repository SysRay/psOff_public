#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceAudiodecCpu);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceAudiodecCpu";

EXPORT SYSV_ABI int32_t sceAudiodecCpuQueryMemSize(SceAudiodecCpuCtrl* pCtrl, SceAudiodecCpuResource* pRes, uint32_t uiCodecType) {
  LOG_USE_MODULE(libSceAudiodecCpu);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudiodecCpuInitDecoder(SceAudiodecCpuCtrl* pCtrl, SceAudiodecCpuResource* pRes, uint32_t uiCodecType) {
  LOG_USE_MODULE(libSceAudiodecCpu);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceAudiodecCpuDecode(SceAudiodecCpuCtrl* pCtrl, SceAudiodecCpuResource* pRes, uint32_t uiCodecType) {
  LOG_USE_MODULE(libSceAudiodecCpu);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}