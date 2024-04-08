#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libkernel_ps2emu);

extern "C" {

EXPORT const char* MODULE_NAME = "libkernel";

SYSV_ABI int __NID(socketpair)() {
  LOG_USE_MODULE(libkernel_ps2emu);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
