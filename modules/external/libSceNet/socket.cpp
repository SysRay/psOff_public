#include "common.h"
#include "core/networking/networking.h"
#include "logging.h"
#include "socketTypes.h"

LOG_DEFINE_MODULE(libSceNet);

extern "C" {

EXPORT SYSV_ABI SceNetId sceNetSocket(const char* name, int family, int type, int protocol) {
  return accessNetworking().socketCreate(name, family, type, protocol);
}

EXPORT SYSV_ABI SceNetId sceNetAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen) {
  return accessNetworking().socketAccept(s, addr, addrlen);
}

EXPORT SYSV_ABI int sceNetBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen) {
  return accessNetworking().socketBind(s, addr, addrlen);
}

EXPORT SYSV_ABI int sceNetConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen) {
  return accessNetworking().socketConnect(s, name, namelen);
}

EXPORT SYSV_ABI int sceNetGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return accessNetworking().socketGetpeername(s, name, namelen);
}

EXPORT SYSV_ABI int sceNetGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return accessNetworking().socketGetsockname(s, name, namelen);
}

EXPORT SYSV_ABI int sceNetGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen) {
  return accessNetworking().socketGetsockopt(s, level, optname, optval, optlen);
}

EXPORT SYSV_ABI int sceNetListen(SceNetId s, int backlog) {
  return accessNetworking().socketListen(s, backlog);
}

EXPORT SYSV_ABI int sceNetRecv(SceNetId s, void* buf, size_t len, int flags) {
  return accessNetworking().socketRecv(s, buf, len, flags);
}

EXPORT SYSV_ABI int sceNetRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen) {
  return accessNetworking().socketRecvfrom(s, buf, len, flags, from, fromlen);
}

EXPORT SYSV_ABI int sceNetRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags) {
  return accessNetworking().socketRecvmsg(s, msg, flags);
}

EXPORT SYSV_ABI int sceNetSend(SceNetId s, const void* msg, size_t len, int flags) {
  return accessNetworking().socketSend(s, msg, len, flags);
}

EXPORT SYSV_ABI int sceNetSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) {
  return accessNetworking().socketSendto(s, msg, len, flags, to, tolen);
}

EXPORT SYSV_ABI int sceNetSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags) {
  return accessNetworking().socketSendmsg(s, msg, flags);
}

EXPORT SYSV_ABI int sceNetSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen) {
  return accessNetworking().socketSetsockopt(s, level, optname, optval, optlen);
}

EXPORT SYSV_ABI int sceNetShutdown(SceNetId s, int how) {
  return accessNetworking().socketShutdown(s, how);
}

EXPORT SYSV_ABI int sceNetSocketClose(SceNetId s) {
  return accessNetworking().socketClose(s);
}

EXPORT SYSV_ABI int sceNetSocketAbort(SceNetId s, int flags) {
  return accessNetworking().socketAbort(s, flags);
}
}
