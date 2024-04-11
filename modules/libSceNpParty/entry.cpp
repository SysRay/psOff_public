#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNpParty);

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNpParty";

EXPORT SYSV_ABI int32_t sceNpPartyInitialize() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceNpPartyCheckCallback() {
  LOG_USE_MODULE(libSceNpParty);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
