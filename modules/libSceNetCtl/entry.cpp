#include "common.h"
#include "types.h"

#include <logging.h>

LOG_DEFINE_MODULE(libSceNetCtl);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNetCtl";

EXPORT SYSV_ABI int sceNetCtlInit(void) {
  return Ok;
}
EXPORT SYSV_ABI void sceNetCtlTerm(void) {}
EXPORT SYSV_ABI int  sceNetCtlCheckCallback(void) {
  return Ok;
}
EXPORT SYSV_ABI int sceNetCtlRegisterCallback(SceNetCtlCallback func, void* arg, int* cid) {
  *cid = 1;
  return Ok;
}
EXPORT SYSV_ABI int sceNetCtlUnregisterCallback(int cid) {
  return Ok;
}
EXPORT SYSV_ABI int sceNetCtlGetResult(int eventType, int* errorCode) {
  return Ok;
}
EXPORT SYSV_ABI int sceNetCtlGetState(int* state) {
  *state = 0;
  return Ok;
}
EXPORT SYSV_ABI int sceNetCtlGetInfo(int code, SceNetCtlInfo* info) {
  LOG_USE_MODULE(libSceNetCtl);

  switch (code) {
    case 2: memset(info->ether_addr.data, 0, sizeof(info->ether_addr.data)); break;
    case 11: info->ip_config = 0; break;
    case 14: {
      auto const count = std::string("127.0.0.1").copy(info->ip_address, 16);

      info->ip_address[count] = '\0';
    } break;
    default: LOG_CRIT(L"unknown code: %d", code);
  }
  return Ok;
}
EXPORT SYSV_ABI int sceNetCtlGetIfStat(SceNetCtlIfStat* ifStat) {
  return Ok;
}
EXPORT SYSV_ABI int sceNetCtlGetNatInfo(SceNetCtlNatInfo* natInfo) {
  natInfo->stunStatus = 1;
  natInfo->natType    = 3;
  natInfo->mappedAddr = 0x7f000001;
  return Ok;
}
}