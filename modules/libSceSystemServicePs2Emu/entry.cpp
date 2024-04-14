#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceSystemServicePs2Emu);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceSystemService";

SYSV_ABI int sceSystemServiceAddLocalProcessForPs2Emu() {
  LOG_USE_MODULE(libSceSystemServicePs2Emu);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
