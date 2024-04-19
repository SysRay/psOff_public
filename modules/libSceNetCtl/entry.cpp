#include "common.h"
#include "core/networking/networking.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNetCtl);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNetCtl";

EXPORT SYSV_ABI int sceNetCtlInit(void) {
  return Ok;
}

EXPORT SYSV_ABI void sceNetCtlTerm(void) {}

EXPORT SYSV_ABI int sceNetCtlCheckCallback(void) {
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
  *state = 3; // IP address obtained
  return Ok;
}

EXPORT SYSV_ABI int sceNetCtlGetInfo(int code, SceNetCtlInfo* info) {
  return accessNetworking().netCtlGetInfo(code, info);
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
