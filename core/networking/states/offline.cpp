#include "offline.h"

#include "logging.h"
#include "modules/libSceNetCtl/codes.h"

LOG_DEFINE_MODULE(OfflineNetworkingCore);

int32_t OfflineNet::netCtlGetInfo(int32_t code, SceNetCtlInfo* info) {
  LOG_USE_MODULE(OfflineNetworkingCore);
  LOG_TRACE(L"netCtlGetInfo(%d, %p)", code, info);
  ::memset(info, 0, sizeof(SceNetCtlInfo));
  return Err::NetCtl::NOT_CONNECTED;
}

SceNetId OfflineNet::resolverCreate(const char* name, int memid, int flags) {
  static SceNetId id = 0;
  return ++id;
}

int32_t OfflineNet::resolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) {
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OfflineNet::resolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) {
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OfflineNet::resolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) {
  return getErr(ErrCode::_ETIMEDOUT);
}

int32_t OfflineNet::resolverGetError(SceNetId rid, int* result) {
  *result = (int)ErrCode::_ETIMEDOUT;
  return Ok;
}

int32_t OfflineNet::resolverDestroy(SceNetId rid) {
  return Ok;
}

int32_t OfflineNet::resolverAbort(SceNetId rid, int flags) {
  return Ok;
}

SceNetId OfflineNet::epollCreate(const char* name, int flags) {
  static SceNetId id = 0;
  return ++id;
}

int32_t OfflineNet::epollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) {
  return Ok;
}

int32_t OfflineNet::epollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) {
  return Ok;
}

int32_t OfflineNet::epollDestroy(SceNetId eid) {
  return Ok;
}

int32_t OfflineNet::epollAbort(SceNetId eid, int flags) {
  return Ok;
}

SceNetId OfflineNet::socketCreate(const char* name, int family, int type, int protocol) {
  *INetworking::getErrnoPtr() = NetErrNo::SCE_NET_EPROTONOSUPPORT;
  return Err::Net::ERROR_EINVAL;
}

SceNetId OfflineNet::socketAccept(SceNetId s, SceNetSockaddr* addr, SceNetSocklen_t* addrlen) {
  return 0;
}

int OfflineNet::socketBind(SceNetId s, const SceNetSockaddr* addr, SceNetSocklen_t addrlen) {
  return Ok;
}

int OfflineNet::socketConnect(SceNetId s, const SceNetSockaddr* name, SceNetSocklen_t namelen) {
  return Ok;
}

int OfflineNet::socketGetpeername(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return Ok;
}

int OfflineNet::socketGetsockname(SceNetId s, SceNetSockaddr* name, SceNetSocklen_t* namelen) {
  return Ok;
}

int OfflineNet::socketGetsockopt(SceNetId s, int level, int optname, void* optval, SceNetSocklen_t* optlen) {
  return 0;
}

int OfflineNet::socketListen(SceNetId s, int backlog) {
  return Ok;
}

int OfflineNet::socketRecv(SceNetId s, void* buf, size_t len, int flags) {
  return 0;
}

int OfflineNet::socketRecvfrom(SceNetId s, void* buf, size_t len, int flags, SceNetSockaddr* from, SceNetSocklen_t* fromlen) {
  return 0;
}

int OfflineNet::socketRecvmsg(SceNetId s, SceNetMsghdr* msg, int flags) {
  return Ok;
}

int OfflineNet::socketSend(SceNetId s, const void* msg, size_t len, int flags) {
  return 0;
}

int OfflineNet::socketSendto(SceNetId s, const void* msg, size_t len, int flags, const SceNetSockaddr* to, SceNetSocklen_t tolen) {
  return 0;
}

int OfflineNet::socketSendmsg(SceNetId s, const SceNetMsghdr* msg, int flags) {
  return Ok;
}

int OfflineNet::socketSetsockopt(SceNetId s, int level, int optname, const void* optval, SceNetSocklen_t optlen) {
  return Ok;
}

int OfflineNet::socketShutdown(SceNetId s, int how) {
  return Ok;
}

int OfflineNet::socketClose(SceNetId s) {
  return Ok;
}

int OfflineNet::socketAbort(SceNetId s, int flags) {
  return Ok;
}
