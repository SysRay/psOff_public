#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceContentExport);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceContentExport";

EXPORT SYSV_ABI int32_t sceContentExportInit(SceContentExportParam* param) {
  return Err::ContentExport::NOT_IMPLEMENTED;
}

EXPORT SYSV_ABI int32_t sceContentExportInit2(SceContentExportParam* param) {
  return Err::ContentExport::NOT_IMPLEMENTED;
}
}
