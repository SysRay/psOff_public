#include "../online.h"

SceNetId OnlineNet::socketCreate(const char* name, int family, int type, int protocol) {
  if (family != SCE_NET_AF_INET) {
    *INetworking::getErrnoPtr() = NetErrNo::SCE_NET_EPROTONOSUPPORT;
    return Err::Net::ERROR_EINVAL;
  }

  switch (type) {
    case SCE_NET_SOCK_STREAM:
    case SCE_NET_SOCK_DGRAM:
    case SCE_NET_SOCK_RAW: return socket(AF_INET, type, protocol);

    default: {
      *INetworking::getErrnoPtr() = NetErrNo::SCE_NET_EPROTONOSUPPORT;
      return Err::Net::ERROR_EPROTOTYPE;
    }
  }
}

SceNetId OnlineNet::socketAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen) {
  int    _addrlen = addrlen ? (int)*addrlen : sizeof(SceNetSockaddr);
  SOCKET _ret     = accept(s, (sockaddr*)addr, &_addrlen);
  if (_ret == INVALID_SOCKET) return INetworking::getLastError();
  *addrlen = (SceNetSocklen_t)_addrlen;
  return (SceNetId)_ret;
}

int OnlineNet::socketBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen) {
  if (bind(s, (const sockaddr*)addr, addrlen) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen) {
  if (connect(s, (const sockaddr*)name, namelen) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  if (getpeername(s, (sockaddr*)name, (int*)namelen) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  if (getsockname(s, (sockaddr*)name, (int*)namelen) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen) {
  int _optlen = optlen ? (int)*optlen : sizeof(SceNetSockaddr);
  int _ret    = getsockopt(s, level, optname, (char*)optval, &_optlen);
  if (_ret == SOCKET_ERROR) return INetworking::getLastError();
  *optlen = (SceNetSocklen_t)_optlen;
  return _ret;
}

int OnlineNet::socketListen(SceNetId s, int backlog) {
  if (listen(s, backlog) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketRecv(SceNetId s, void* buf, size_t len, int flags) {
  int _ret = recv(s, (char*)buf, (int)len, flags);
  if (_ret == SOCKET_ERROR) return INetworking::getLastError();
  return _ret;
}

int OnlineNet::socketRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen) {
  int _fromlen = fromlen ? (int)*fromlen : sizeof(SceNetSockaddr);
  int _ret     = recvfrom(s, (char*)buf, (int)len, flags, (sockaddr*)from, &_fromlen);
  if (_ret == SOCKET_ERROR) return INetworking::getLastError();
  *fromlen = (SceNetSocklen_t)_fromlen;
  return _ret;
}

int OnlineNet::socketRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags) {
  return Ok;
}

int OnlineNet::socketSend(SceNetId s, const void* msg, size_t len, int flags) {
  int _ret;
  if ((_ret = send(s, (const char*)msg, (int)len, flags)) == SOCKET_ERROR) return INetworking::getLastError();
  return _ret;
}

int OnlineNet::socketSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) {
  int _ret;
  if ((_ret = sendto(s, (const char*)msg, (int)len, flags, (const sockaddr*)to, (int)tolen)) == SOCKET_ERROR) return INetworking::getLastError();
  return _ret;
}

int OnlineNet::socketSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags) {
  return Ok;
}

int OnlineNet::socketSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen) {
  if (setsockopt(s, level, optname, (char*)optval, (int)optlen) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketShutdown(SceNetId s, int how) {
  if (shutdown(s, how) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketClose(SceNetId s) {
  if (closesocket(s) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::socketAbort(SceNetId s, int flags) {
  return Ok;
}
