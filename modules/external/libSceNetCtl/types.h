#pragma once
#include "../libSceNet/types.h"
#include "codes.h"
#include "utility/utility.h"

union SceNetCtlInfo {
  uint32_t        device;
  SceNetEtherAddr ether_addr;
  uint32_t        mtu;
  uint32_t        link;
  SceNetEtherAddr bssid;
  char            ssid[32 + 1];
  uint32_t        wifi_security;
  uint8_t         rssi_dbm;
  uint8_t         rssi_percentage;
  uint8_t         channel;
  uint32_t        ip_config;
  char            dhcp_hostname[255 + 1];
  char            pppoe_auth_name[127 + 1];
  char            ip_address[16];
  char            netmask[16];
  char            default_route[16];
  char            primary_dns[16];
  char            secondary_dns[16];
  uint32_t        http_proxy_config;
  char            http_proxy_server[255 + 1];
  uint16_t        http_proxy_port;
};

struct SceNetCtlNatInfo {
  unsigned int size;
  int          stunStatus;
  int          natType;
  uint32_t     mappedAddr;
};

using SceNetCtlCallback = SYSV_ABI void (*)(int eventType, void* arg);

struct SceNetCtlIfStat {
  uint32_t device;
  uint64_t txBytes;
  uint64_t rxBytes;
};