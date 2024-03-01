#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceSysmodule);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSysmodule";

EXPORT SYSV_ABI int sceSysmoduleLoadModule(uint16_t id) {
  LOG_USE_MODULE(libSceSysmodule);
  LOG_DEBUG(L"id = 0x%lx", id);
  return Ok;
}

EXPORT SYSV_ABI int sceSysmoduleUnloadModule(uint16_t id) {
  LOG_USE_MODULE(libSceSysmodule);
  LOG_DEBUG(L"id = 0x%lx", id);
  return Ok;
}

EXPORT SYSV_ABI int sceSysmoduleIsLoaded(uint16_t id) {
  LOG_USE_MODULE(libSceSysmodule);
  LOG_DEBUG(L"id = 0x%lx", id);
  return Ok;
}

EXPORT SYSV_ABI int sceSysmoduleLoadModuleInternalWithArg(uint16_t id, int arg1, int arg2, int arg3, int* ret) {
  *ret = 0;

  return Ok;
}
}