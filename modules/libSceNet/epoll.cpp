#include "common.h"
#include "logging.h"
#include "types.h"
#include "wepoll.h"

#include <WinSock2.h>
#include <unordered_map>

LOG_DEFINE_MODULE(libSceNet);

namespace {
union SceNetEpollData {
  void*    ptr;
  uint32_t u32;
};

constexpr int SCE_NET_EPOLLIN     = 0x00000001;
constexpr int SCE_NET_EPOLLOUT    = 0x00000002;
constexpr int SCE_NET_EPOLLERR    = 0x00000008;
constexpr int SCE_NET_EPOLLHUP    = 0x00000010;
constexpr int SCE_NET_EPOLLDESCID = 0x00010000;

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

static inline int32_t sce_GetLastError() {
  auto win_err = (uint32_t)GetLastError();
  if (win_err == WSANOTINITIALISED) return Err::ERROR_ENOTINIT;
  return (0x80000000 | (0x041 << 16) | (0x0100 | win_err));
}

static inline uint32_t sce_map_epoll_events(uint32_t events) {
  LOG_USE_MODULE(libSceNet);
  uint32_t _events = 0;
  if (events & SCE_NET_EPOLLIN) _events |= EPOLLIN;
  if (events & SCE_NET_EPOLLOUT) _events |= EPOLLOUT;
  if (events & SCE_NET_EPOLLERR) _events |= EPOLLERR;
  if (events & SCE_NET_EPOLLHUP) _events |= EPOLLHUP;
  if (events & SCE_NET_EPOLLDESCID) LOG_CRIT(L"Unhandled epoll event SCE_NET_EPOLLDESCID");
  return _events;
}

// Probably the safest way, since sizeof(SceNetId) < sizeof(HANDLE)
// We cannot guarantee that this HANDLE value will not exceed int32_t
static std::unordered_map<SceNetId, HANDLE> map;

static HANDLE sce_map_id_get(SceNetId id) {
  if (map.find(id) != map.end()) return map[id];
  return INVALID_HANDLE_VALUE;
}

static HANDLE sce_map_id_del(SceNetId id) {
  auto it = map.find(id);
  if (it != map.end()) {
    auto value = map[id];
    map.erase(it);
    return value;
  }
  return INVALID_HANDLE_VALUE;
}
} // namespace

extern "C" {
EXPORT SYSV_ABI SceNetId sceNetEpollCreate(const char* name, int flags) {
  static SceNetId id = 0;
  HANDLE          ep;

  if ((ep = epoll_create1(flags)) != NULL && ep != INVALID_HANDLE_VALUE) {
    map[++id] = ep;
    return id;
  }

  return sce_GetLastError();
}

EXPORT SYSV_ABI int sceNetEpollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) {
  epoll_event ev {
      .events = sce_map_epoll_events(event->events),
      .data   = {.ptr = event->data.ptr},
  };

  if (epoll_ctl(sce_map_id_get(eid), op, id, &ev) == SOCKET_ERROR) return sce_GetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetEpollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) {
  epoll_event ev {
      .events = sce_map_epoll_events(events->events),
      .data   = {.ptr = events->data.ptr},
  };
  if (epoll_wait(sce_map_id_get(eid), &ev, maxevents, timeout) == SOCKET_ERROR) return sce_GetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetEpollDestroy(SceNetId eid) {
  if (epoll_close(sce_map_id_del(eid)) == SOCKET_ERROR) return sce_GetLastError();
  return Ok;
}

EXPORT SYSV_ABI int sceNetEpollAbort(SceNetId eid, int flags) {
  return Ok;
}
}
