#include "common.h"
#include "logging.h"
#include "types.h"

#include <WS2tcpip.h>
#include <WinSock2.h>

LOG_DEFINE_MODULE(libSceNet);

namespace {
static inline int32_t sce_WSAGetLastError() {
  auto win_err = (uint32_t)WSAGetLastError();
  if (win_err == WSANOTINITIALISED) return Err::ERROR_ENOTINIT;
  return (0x80000000 | (0x041 << 16) | (0x0100 | win_err));
}

static thread_local int g_net_errno = 0;

} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNet";

EXPORT SYSV_ABI int* sceNetErrnoLoc() {
  return &g_net_errno;
}

EXPORT SYSV_ABI int sceNetInit(void) {
  WSADATA data = {};
  if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetTerm(void) {
  if (WSACleanup() == SOCKET_ERROR) return sce_WSAGetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetPoolCreate(const char* name, int size, int flags) {
  static int id = 0;
  return ++id;
}

EXPORT SYSV_ABI int sceNetPoolDestroy(int memid) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetMemoryPoolStats(int memid, SceNetMemoryPoolStats* stat) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowIfconfig(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowIfconfigWithMemory(int memid) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowRoute(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowRouteWithMemory(int memid) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowNetstat(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowNetstatWithMemory(int memid) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowPolicy(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetShowPolicyWithMemory(int memid) {
  return Ok;
}

EXPORT SYSV_ABI const char* sceNetInetNtop(int af, const void* src, char* dst, size_t size) {
  return InetNtopA(af, src, dst, size);
}

EXPORT SYSV_ABI int sceNetInetPton(int af, const char* src, void* dst) {
  return InetPtonA(af, src, dst);
}

EXPORT SYSV_ABI uint64_t sceNetHtonll(uint64_t host64) {
  return htonll(host64);
}

EXPORT SYSV_ABI uint32_t sceNetHtonl(uint32_t host32) {
  return htonl(host32);
}

EXPORT SYSV_ABI uint16_t sceNetHtons(uint16_t host16) {
  return htons(host16);
}

EXPORT SYSV_ABI uint64_t sceNetNtohll(uint64_t net64) {
  return ntohll(net64);
}

EXPORT SYSV_ABI uint32_t sceNetNtohl(uint32_t net32) {
  return ntohl(net32);
}

EXPORT SYSV_ABI uint16_t sceNetNtohs(uint16_t net16) {
  return ntohs(net16);
}

EXPORT SYSV_ABI int sceNetEtherStrton(const char* str, SceNetEtherAddr* n) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetEtherNtostr(const SceNetEtherAddr* n, char* str, size_t len) {
  auto const count = std::string("00:00:00:00:00:00").copy(str, len);
  str[count]       = '\0';
  return Ok;
}

EXPORT SYSV_ABI int sceNetGetMacAddress(SceNetEtherAddr* addr, int flags) {
  memset(addr->data, 0, sizeof(addr->data));
  return Ok;
}
}
