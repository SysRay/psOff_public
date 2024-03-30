#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpCommerce);

namespace {
static SceCommonDialogStatus g_curStatus = SceCommonDialogStatus::NONE;
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpCommerce";

EXPORT SYSV_ABI int32_t sceNpCommerceDialogInitialize() {
  g_curStatus = SceCommonDialogStatus::INITIALIZED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpCommerceDialogTerminate(void) {
  g_curStatus = SceCommonDialogStatus::NONE;
  return Ok;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceNpCommerceDialogUpdateStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI SceCommonDialogStatus sceNpCommerceDialogGetStatus(void) {
  return g_curStatus;
}

EXPORT SYSV_ABI int32_t sceNpCommerceDialogGetResult(void) {
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpCommerceDialogOpen(const void* param) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpCommerceDialogClose(void) {
  g_curStatus = SceCommonDialogStatus::FINISHED;
  return Ok;
}
}
