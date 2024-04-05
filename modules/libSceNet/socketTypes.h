#pragma once
#include "types.h"

constexpr int SCE_NET_MSG_PEEK         = 0x00000002;
constexpr int SCE_NET_MSG_WAITALL      = 0x00000040;
constexpr int SCE_NET_MSG_DONTWAIT     = 0x00000080;
constexpr int SCE_NET_MSG_USECRYPTO    = 0x00100000;
constexpr int SCE_NET_MSG_USESIGNATURE = 0x00200000;
constexpr int SCE_NET_MSG_PEEKLEN      = (0x00400000 | SCE_NET_MSG_PEEK);

constexpr int SCE_NET_SHUT_RD   = 0;
constexpr int SCE_NET_SHUT_WR   = 1;
constexpr int SCE_NET_SHUT_RDWR = 2;

constexpr int SCE_NET_SOCKET_ABORT_FLAG_RCV_PRESERVATION       = 0x00000001;
constexpr int SCE_NET_SOCKET_ABORT_FLAG_SND_PRESERVATION       = 0x00000002;
constexpr int SCE_NET_SOCKET_ABORT_FLAG_SND_PRESERVATION_AGAIN = 0x00000004;

constexpr int SCE_NET_AF_INET = 0x00000002;

constexpr int SCE_NET_SOCK_STREAM     = 1;
constexpr int SCE_NET_SOCK_DGRAM      = 2;
constexpr int SCE_NET_SOCK_RAW        = 3;
constexpr int SCE_NET_SOCK_DGRAM_P2P  = 6;
constexpr int SCE_NET_SOCK_STREAM_P2P = 10;

struct SceNetLinger {
  int l_onoff;  /* on/off */
  int l_linger; /* linger time [sec] */
};

struct SceNetSockaddr {
  uint8_t          sa_len;
  SceNetSaFamily_t sa_family;
  char             sa_data[14];
};

struct SceNetIovec {
  void*  iov_base;
  size_t iov_len;
};

struct SceNetMsghdr {
  void*           msg_name;
  SceNetSocklen_t msg_namelen;
  SceNetIovec*    msg_iov;
  int             msg_iovlen;
  void*           msg_control;
  SceNetSocklen_t msg_controllen;
  int             msg_flags;
};

struct SceNetUdpSndOnSuspend {
  int             onoff;
  SceNetSockaddr* addr;
  SceNetSocklen_t addrlen;
#define SCE_NET_UDP_SND_ON_SUSPEND_DATALEN_MAX (512)
  void*           data;
  SceNetSocklen_t datalen;
};
