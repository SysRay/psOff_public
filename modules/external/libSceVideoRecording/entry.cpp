#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceVideoRecording);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceVideoRecording";

EXPORT SYSV_ABI int32_t sceVideoRecordingSetInfo() {
  LOG_USE_MODULE(libSceVideoRecording);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
