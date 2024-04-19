#include "common.h"
#include "logging.h"
#include "types.h"
#include "core/networking/networking.h"

extern "C" {
EXPORT SYSV_ABI SceNetId sceNetEpollCreate(const char* name, int flags) {
  return accessNetworking().epollCreate(name, flags);
}

EXPORT SYSV_ABI int sceNetEpollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) {
  return accessNetworking().epollControl(eid, op, id, event);
}

EXPORT SYSV_ABI int sceNetEpollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) {
  return accessNetworking().epollWait(eid, events, maxevents, timeout);
}

EXPORT SYSV_ABI int sceNetEpollDestroy(SceNetId eid) {
  return accessNetworking().epollDestroy(eid);
}

EXPORT SYSV_ABI int sceNetEpollAbort(SceNetId eid, int flags) {
  return accessNetworking().epollAbort(eid, flags);
}
}
