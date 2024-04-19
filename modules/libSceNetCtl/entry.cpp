#include "common.h"
#include "config_emu.h"
#include "logging.h"
#include "types.h"

// Sigh, Microsoft
// clang-format off
#include <Windows.h>
#include <WinSock2.h>
#include <iphlpapi.h>
// clang-format on

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
  PIP_ADAPTER_INFO      pAda     = nullptr; // Adapters head
  PIP_ADAPTER_INFO      pCurrAda = nullptr; // Used adapter
  ULONG                 uAdaBufLen;
  PIP_ADAPTER_ADDRESSES pAddr     = nullptr; // Addresses head
  PIP_ADAPTER_ADDRESSES pCurrAddr = nullptr; // Used adapter addr
  ULONG                 uAddrBufLen;

  auto initAdapterInfo = [&pAda, &pCurrAda, &uAdaBufLen]() {
    LOG_USE_MODULE(libSceNetCtl);
    pAda       = nullptr;
    uAdaBufLen = 0;

    if (GetAdaptersInfo(nullptr, &uAdaBufLen) == ERROR_BUFFER_OVERFLOW) { // Obtaining buffer length
      pAda = (PIP_ADAPTER_INFO)malloc(uAdaBufLen);
    } else {
      LOG_ERR(L"Failed to obtain size needed for GetAdaptersInfo buffer!");
      return Err::ERROR_EFAULT;
    }

    if (auto ret = GetAdaptersInfo(pAda, &uAdaBufLen)) {
      free(pAda);
      pAda = nullptr;
      LOG_ERR(L"GetAdaptersInfo failed: %x", ret);
      return Err::ERROR_EFAULT;
    }

    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);
    std::string macAddr;

    if (!getJsonParam(jData, "netMAC", macAddr)) {
      macAddr = "00:00:00:00:00:00";
    }

    pCurrAda = pAda;
    if (macAddr == "00:00:00:00:00:00") {
      while (pCurrAda != nullptr) { // Find the first adapter with non-zero ip address
        if (strcmp(pCurrAda->IpAddressList.IpAddress.String, "0.0.0.0") != 0) break;
        pCurrAda = pCurrAda->Next;
      }
    } else {
      uint8_t exMAC[6] = {0}; // Expected MAC address
      if (sscanf_s(macAddr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &exMAC[0], &exMAC[1], &exMAC[2], &exMAC[3], &exMAC[4], &exMAC[5]) == 6) {
        while (pCurrAda != nullptr) { // Find the adapter with specified MAC address
          if (pCurrAda->AddressLength == sizeof(exMAC) && memcmp(pCurrAda->Address, exMAC, sizeof(exMAC)) == 0) break;
          pCurrAda = pCurrAda->Next;
        }
      } else {
        LOG_ERR(L"initAdapterInfo: failed to parse netMAC value from config!");
      }
    }

    if (pCurrAda != nullptr) return Ok;
    LOG_ERR(L"initAdapterInfo: failed to find suitable network adapter!");
    free(pAda);
    pAda = pCurrAda = nullptr;
    return Err::ERROR_EFAULT;
  };

  auto initAdapterAddr = [&pAddr, &pCurrAddr, &uAddrBufLen, &pAda, &pCurrAda, initAdapterInfo]() {
    LOG_USE_MODULE(libSceNetCtl);
    pAddr       = nullptr;
    uAddrBufLen = 0;

    if (GetAdaptersAddresses(AF_INET, 0, nullptr, nullptr, &uAddrBufLen) == ERROR_BUFFER_OVERFLOW) { // Obtaining buffer length
      pAddr = (PIP_ADAPTER_ADDRESSES)malloc(uAddrBufLen);
    } else {
      LOG_ERR(L"Failed to obtain size needed for GetAdaptersAddresses buffer!");
      return Err::ERROR_EFAULT;
    }

    if (auto ret = GetAdaptersAddresses(AF_INET, 0, nullptr, pAddr, &uAddrBufLen)) {
      free(pAddr);
      pAddr = nullptr;
      LOG_ERR(L"GetAdaptersAddresses failed: %d (%d)", ret, uAddrBufLen);
      return Err::ERROR_EFAULT;
    }

    if (auto ret = initAdapterInfo()) { // Trying to find suitable adapter first
      free(pAddr);
      pAddr = nullptr;
      return ret;
    }

    pCurrAddr = pAddr;
    while (pCurrAddr != nullptr) { // Adapter found, okay. Now we should match this adapter with its address
      if (pCurrAda->AddressLength == pCurrAddr->PhysicalAddressLength && memcmp(pCurrAddr->PhysicalAddress, pCurrAda->Address, pCurrAda->AddressLength) == 0)
        break;
      pCurrAddr = pAddr->Next;
    }

    if (pCurrAddr != nullptr) return Ok;
    LOG_ERR(L"initAdapterAddr: failed to find suitable network address!");
    free(pAddr);
    free(pAda);
    pAddr = pCurrAddr = nullptr;
    pAda = pCurrAda = nullptr;
    return Ok;
  };

  switch (code) {
    case 1: info->device = 0; break;
    case 2: {
      if (auto ret = initAdapterInfo()) return ret;
      memcpy(info->ether_addr.data, pCurrAda->Address, pCurrAda->AddressLength);
    } break;
    case 3: {
      if (auto ret = initAdapterAddr()) return ret;
      info->mtu = (uint32_t)pCurrAddr->Mtu;
    } break;
    case 4: info->link = 1; break;
    case 5:
    case 6:
    case 7: // Big and scary fallthrough
    case 8:
    case 9:
    case 10: return Err::ERROR_NOT_AVAIL; // All these above for WiFi
    case 11: info->ip_config = 0; break;
    case 12: {
      DWORD size = sizeof(info->dhcp_hostname);
      if (GetComputerNameA(info->dhcp_hostname, &size)) break; // todo: shouldn't be DNS suffix there?
      info->dhcp_hostname[0] = '\0';
    } break;
    case 13: {
      auto const count = std::string("ps4_pppoe").copy(info->pppoe_auth_name, 128); // Huh?

      info->pppoe_auth_name[count] = '\0';
    } break;
    case 14: {
      if (auto ret = initAdapterInfo()) return ret;
      auto const count = std::string(pCurrAda->IpAddressList.IpAddress.String).copy(info->ip_address, 16);

      info->ip_address[count] = '\0';
    } break;
    case 15: {
      if (auto ret = initAdapterInfo()) return ret;
      auto const count = std::string(pCurrAda->IpAddressList.IpMask.String).copy(info->netmask, 16);

      info->netmask[count] = '\0';
    } break;
    case 16: {
      if (auto ret = initAdapterInfo()) return ret;
      auto const count = std::string(pCurrAda->GatewayList.IpAddress.String).copy(info->default_route, 16);

      info->default_route[count] = '\0';
    } break;
    case 17: {
      if (auto ret = initAdapterAddr()) return ret;
      auto dns = pCurrAddr->FirstDnsServerAddress;
      if (dns == nullptr) {
        info->primary_dns[0] = '\0';
        break;
      }
      auto  addr   = &pCurrAddr->FirstDnsServerAddress->Address;
      DWORD pdnssz = sizeof(info->primary_dns);
      if (WSAAddressToStringA(addr->lpSockaddr, addr->iSockaddrLength, nullptr, info->primary_dns, &pdnssz) == SOCKET_ERROR)
        info->primary_dns[0] = '\0'; // todo return error?
    } break;
    case 18: {
      if (auto ret = initAdapterAddr()) return ret;
      auto dns = pCurrAddr->FirstDnsServerAddress;
      if (dns == nullptr) {
        info->secondary_dns[0] = '\0';
        break;
      }
      if (dns->Next != nullptr) dns = dns->Next; // Use the secondary DNS if present
      auto  addr   = &dns->Address;
      DWORD pdnssz = sizeof(info->secondary_dns);
      if (WSAAddressToStringA(addr->lpSockaddr, addr->iSockaddrLength, nullptr, info->secondary_dns, &pdnssz) == SOCKET_ERROR)
        info->secondary_dns[0] = '\0'; // todo return error?
    } break;
    case 19: info->http_proxy_config = 0; break;
    case 20: *info->http_proxy_server = '\0'; break;
    case 21: info->http_proxy_port = 0; break;
    default: LOG_CRIT(L"unknown code: %d", code);
  }

  if (pAda != nullptr) free(pAda);
  if (pAddr != nullptr) free(pAddr);

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
