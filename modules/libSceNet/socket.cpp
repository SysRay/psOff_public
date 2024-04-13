#include "common.h"
#include "logging.h"
#include "socketTypes.h"

#include <WS2tcpip.h>
#include <mutex>

LOG_DEFINE_MODULE(libSceNet);

namespace {
extern "C" int* sceNetErrnoLoc();

static inline int32_t sce_WSAGetLastError() {
  auto win_err = (uint32_t)WSAGetLastError();
  if (win_err == WSANOTINITIALISED) return Err::ERROR_ENOTINIT;
  return (0x80000000 | (0x041 << 16) | (0x0100 | win_err));
}
} // namespace

extern "C" {

EXPORT SYSV_ABI SceNetId sceNetSocket(const char* name, int family, int type, int protocol) {
  if (family != SCE_NET_AF_INET) {
    *sceNetErrnoLoc() = NetErrNo::SCE_NET_EPROTONOSUPPORT;
    return Err::ERROR_EINVAL;
  }

  switch (type) {
    case SCE_NET_SOCK_STREAM:
    case SCE_NET_SOCK_DGRAM:
    case SCE_NET_SOCK_RAW: return socket(AF_INET, type, protocol);

    default: {
      *sceNetErrnoLoc() = NetErrNo::SCE_NET_EPROTONOSUPPORT;
      return Err::ERROR_EPROTOTYPE;
    }
  }

  return Ok;
}

EXPORT SYSV_ABI SceNetId sceNetAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen) {
  int    _addrlen = addrlen ? (int)*addrlen : sizeof(SceNetSockaddr);
  SOCKET _ret     = accept(s, (sockaddr*)addr, &_addrlen);
  if (_ret == INVALID_SOCKET) return sce_WSAGetLastError();
  *addrlen = (SceNetSocklen_t)_addrlen;
  return (SceNetId)_ret;
}

EXPORT SYSV_ABI int sceNetBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen) {
  if (bind(s, (const sockaddr*)addr, addrlen) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen) {
  if (connect(s, (const sockaddr*)name, namelen) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  if (getpeername(s, (sockaddr*)name, (int*)namelen) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  if (getsockname(s, (sockaddr*)name, (int*)namelen) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen) {
  int _optlen = optlen ? (int)*optlen : sizeof(SceNetSockaddr);
  int _ret    = getsockopt(s, level, optname, (char*)optval, &_optlen);
  if (_ret == SOCKET_ERROR) return sce_WSAGetLastError();
  *optlen = (SceNetSocklen_t)_optlen;
  return _ret;
}

EXPORT SYSV_ABI int sceNetListen(SceNetId s, int backlog) {
  if (listen(s, backlog) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetRecv(SceNetId s, void* buf, size_t len, int flags) {
  int _ret = recv(s, (char*)buf, (int)len, flags);
  if (_ret == SOCKET_ERROR) return sce_WSAGetLastError();
  return _ret;
}

EXPORT SYSV_ABI int sceNetRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen) {
  int _fromlen = fromlen ? (int)*fromlen : sizeof(SceNetSockaddr);
  int _ret     = recvfrom(s, (char*)buf, (int)len, flags, (sockaddr*)from, &_fromlen);
  if (_ret == SOCKET_ERROR) return sce_WSAGetLastError();
  *fromlen = (SceNetSocklen_t)_fromlen;
  return _ret;
}

EXPORT SYSV_ABI int sceNetRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSend(SceNetId s, const void* msg, size_t len, int flags) {
  int _ret;
  if ((_ret = send(s, (const char*)msg, (int)len, flags)) == SOCKET_ERROR) return sce_WSAGetLastError();
  return _ret;
}

EXPORT SYSV_ABI int sceNetSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) {
  int _ret;
  if ((_ret = sendto(s, (const char*)msg, (int)len, flags, (const sockaddr*)to, (int)tolen)) == SOCKET_ERROR) return sce_WSAGetLastError();
  return _ret;
}

EXPORT SYSV_ABI int sceNetSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen) {
  if (setsockopt(s, level, optname, (char*)optval, (int)optlen) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetShutdown(SceNetId s, int how) {
  if (shutdown(s, how) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetSocketClose(SceNetId s) {
  if (closesocket(s) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetSocketAbort(SceNetId s, int flags) {
  return Ok;
}
}
