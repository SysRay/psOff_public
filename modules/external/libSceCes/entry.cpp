#include "codes.h"
#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceCes);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceCes";

EXPORT SYSV_ABI int32_t sceCesUcsProfileInitSJis1997Cp932() {
  LOG_USE_MODULE(libSceCes);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int32_t sceCesMbcsUcsContextInit() {
  LOG_USE_MODULE(libSceCes);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
