#pragma once
#include "codes.h"

typedef uint32_t SceNetInAddr_t;
typedef uint16_t SceNetInPort_t;
typedef uint8_t  SceNetSaFamily_t;
typedef uint32_t SceNetSocklen_t;

typedef int SceNetId;

enum class SceStunCheckResult {
  Unchecked = 0,
  Failed    = 1,
  Ok        = 2

};

enum class SceNatType {
  Type1 = 1,
  Type2 = 2,
  Type3 = 3

};

enum class SceIpConfig {
  Dhcp   = 0,
  Static = 1,
  Pppoe  = 2

};

enum class SceHttpProxyConfig {
  Off = 0,
  On  = 1

};

enum class SceNetworkState {
  Disconnected = 0,
  Connecting   = 1,
  Ipobtaining  = 2,
  Ipobtained   = 3

};

enum class SceEventType {
  Disconnected          = 1,
  DisconnectReqFinished = 2,
  Ipobtained            = 3

};

enum class SceInfoCode {
  Device          = 1,
  EtherAddr       = 2,
  Mtu             = 3,
  Link            = 4,
  Bssid           = 5,
  Ssid            = 6,
  WiFiSecurity    = 7,
  RssiDbm         = 8,
  RssiPercentage  = 9,
  Channel         = 10,
  IPConfig        = 11,
  DhcpHostname    = 12,
  PppoeAuthName   = 13,
  IPAddress       = 14,
  Netmask         = 15,
  DefaultRoute    = 16,
  PrimaryDNS      = 17,
  SecondaryDNS    = 18,
  HttpProxyConfig = 19,
  HttpProxyServer = 20,
  HttpProxyPort   = 21,
  Reserved1       = 22,
  Reserved2       = 23

};

enum class SceDevice {
  Wired    = 0,
  Wireless = 1

};

enum class SceLink {
  Disconnected = 0,
  Connected    = 1

};

enum class SceWiFiSecurity {
  Noauth        = 0,
  Wep           = 1,
  WPAPSKWPA2PSK = 2,
  WPAPSKTKIP    = 3,
  WPAPSKAES     = 4,
  WPA2PSKTKIP   = 5,
  WPA2PSKAES    = 6,
  Unsupported   = 7

};

struct SceNetMemoryPoolStats {
  size_t poolSize;
  size_t maxInuseSize;
  size_t currentInuseSize;
};

typedef struct SceNetEtherAddr {
  uint8_t data[SCE_NET_ETHER_ADDR_LEN];
} SceNetEtherAddr;