#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNet);

namespace {
static thread_local int g_net_errno = 0;

} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceNet";

EXPORT SYSV_ABI int* sceNetErrnoLoc() {
  return &g_net_errno;
}

EXPORT SYSV_ABI int sceNetInit(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetTerm(void) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetPoolCreate(const char* name, int size, int flags) {
  static int id = 0;
  return ++id;
  return Ok;
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

const char* sceNetInetNtop(int af, const void* src, char* dst, size_t size);

EXPORT SYSV_ABI int sceNetInetPton(int af, const char* src, void* dst) {
  *static_cast<uint32_t*>(dst) = 0x7f000001;
  return Ok;
}

EXPORT SYSV_ABI uint64_t sceNetHtonll(uint64_t host64) {
  return Ok;
}

EXPORT SYSV_ABI uint32_t sceNetHtonl(uint32_t host32) {
  return Ok;
}

EXPORT SYSV_ABI uint16_t sceNetHtons(uint16_t host16) {
  return Ok;
}

EXPORT SYSV_ABI uint64_t sceNetNtohll(uint64_t net64) {
  return Ok;
}

EXPORT SYSV_ABI uint32_t sceNetNtohl(uint32_t net32) {
  return Ok;
}

EXPORT SYSV_ABI uint16_t sceNetNtohs(uint16_t net16) {
  return Ok;
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