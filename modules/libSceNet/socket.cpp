#include "common.h"
#include "logging.h"
#include "socketTypes.h"
LOG_DEFINE_MODULE(libSceNet);

namespace {} // namespace

extern "C" {

EXPORT SYSV_ABI SceNetId sceNetSocket(const char* name, int domain, int type, int protocol) {
  return Ok;
}

EXPORT SYSV_ABI SceNetId sceNetAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen) {
  if (addr != nullptr)
    *addr = SceNetSockaddr {
        sizeof(SceNetSockaddr),
        2,
        {80, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    };

  return Ok;
}

EXPORT SYSV_ABI int sceNetBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetListen(SceNetId s, int backlog) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetRecv(SceNetId s, void* buf, size_t len, int flags) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSend(SceNetId s, const void* msg, size_t len, int flags) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShutdown(SceNetId s, int how) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSocketClose(SceNetId s) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetSocketAbort(SceNetId s, int flags) {
  return Ok;
}
}