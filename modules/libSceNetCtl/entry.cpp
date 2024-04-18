#include "common.h"
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
  LOG_USE_MODULE(libSceNetCtl);
  // todo: write the actual info there from the host system

  switch (code) {
    case 1: info->device = 0; break;
    case 2: memset(info->ether_addr.data, 0, sizeof(info->ether_addr.data)); break;
    case 3: info->mtu = 1500; break;
    case 4: info->link = 1; break; // Should we imitate disconnected link for now?
    case 5:
    case 6:
    case 7: // Big and scary fallthrough
    case 8:
    case 9:
    case 10: return Err::ERROR_NOT_AVAIL; // All these above for WiFi
    case 11: info->ip_config = 0; break;
    case 12: {
      auto const count = std::string("ps4.local").copy(info->dhcp_hostname, 128);

      info->dhcp_hostname[count] = '\0';
    } break;
    case 13: {
      auto const count = std::string("ps4_pppoe").copy(info->pppoe_auth_name, 128);

      info->pppoe_auth_name[count] = '\0';
    } break;
    case 14: {
      auto const count = std::string("192.168.13.37").copy(info->ip_address, 16);

      info->ip_address[count] = '\0';
    } break;
    case 15: {
      auto const count = std::string("255.255.255.0").copy(info->netmask, 16);

      info->netmask[count] = '\0';
    } break;
    case 16: {
      auto const count = std::string("192.168.13.1").copy(info->default_route, 16);

      info->default_route[count] = '\0';
    } break;
    case 17: {
      auto const count = std::string("192.168.13.1").copy(info->primary_dns, 16);

      info->primary_dns[count] = '\0';
    } break;
    case 18: {
      auto const count = std::string("8.8.8.8").copy(info->secondary_dns, 16);

      info->primary_dns[count] = '\0';
    } break;
    case 19: info->http_proxy_config = 0; break;
    case 20: *info->http_proxy_server = '\0'; break;
    case 21: info->http_proxy_port = 0; break;
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
