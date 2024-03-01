#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceNet);

namespace {
union SceNetEpollData {
  void*    ptr;
  uint32_t u32;
};

enum class SCE_NET_EpollEvent : int {
  EPOLLIN     = 0x00000001,
  EPOLLOUT    = 0x00000002,
  EPOLLERR    = 0x00000008,
  EPOLLHUP    = 0x00000010,
  EPOLLDESCID = 0x00010000,

};

struct SceNetEpollEvent {
  uint32_t        events;
  uint32_t        reserved;
  uint64_t        ident; /* OUT */
  SceNetEpollData data;
};

constexpr int SCE_NET_EPOLL_CTL_ADD = 1;
constexpr int SCE_NET_EPOLL_CTL_MOD = 2;
constexpr int SCE_NET_EPOLL_CTL_DEL = 3;

constexpr int SCE_NET_EPOLL_ABORT_FLAG_PRESERVATION = 0x00000001;
} // namespace

extern "C" {
EXPORT SYSV_ABI SceNetId sceNetEpollCreate(const char* name, int flags) {
  static int id = 0;
  return ++id;
}

EXPORT SYSV_ABI int sceNetEpollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetEpollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetEpollDestroy(SceNetId eid) {
  return Ok;
}

EXPORT SYSV_ABI int sceNetEpollAbort(SceNetId eid, int flags) {
  return Ok;
}
}