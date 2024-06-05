#include "common.h"
#include "internal/networking/networking.h"
#include "logging.h"
#include "resolverTypes.h"
#include "socketTypes.h"

LOG_DEFINE_MODULE(libSceNet);

extern "C" {
EXPORT SYSV_ABI SceNetId sceNetResolverCreate(const char* name, int memid, int flags) {
  return accessNetworking().resolverCreate(name, memid, flags);
}

EXPORT SYSV_ABI int sceNetResolverStartNtoa(SceNetId rid, const char* hostname, SceNetInAddr_t* addr, int timeout, int retries, int flags) {
  return accessNetworking().resolverStartNtoa(rid, hostname, addr, timeout, retries, flags);
}

EXPORT SYSV_ABI int sceNetResolverStartAton(SceNetId rid, const SceNetInAddr_t* addr, char* hostname, int len, int timeout, int retry, int flags) {
  return accessNetworking().resolverStartAton(rid, addr, hostname, len, timeout, retry, flags);
}

EXPORT SYSV_ABI int sceNetResolverStartNtoaMultipleRecords(SceNetId rid, const char* hostname, SceNetResolverInfo* info, int timeout, int retries, int flags) {
  return accessNetworking().resolverStartNtoaMultipleRecords(rid, hostname, info, timeout, retries, flags);
}

EXPORT SYSV_ABI int sceNetResolverGetError(SceNetId rid, int* result) {
  return accessNetworking().resolverGetError(rid, result);
}

EXPORT SYSV_ABI int sceNetResolverDestroy(SceNetId rid) {
  return accessNetworking().resolverDestroy(rid);
}

EXPORT SYSV_ABI int sceNetResolverAbort(SceNetId rid, int flags) {
  return accessNetworking().resolverAbort(rid, flags);
}
}
