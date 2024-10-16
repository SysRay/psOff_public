#include "../online.h"
#include "logging.h"

#include <WinSock2.h>
#include <unordered_map>
#include <wepoll.h>

LOG_DEFINE_MODULE(OnlineNetCore_epoll);

namespace {
constexpr int SCE_NET_EPOLLIN     = 0x00000001;
constexpr int SCE_NET_EPOLLOUT    = 0x00000002;
constexpr int SCE_NET_EPOLLERR    = 0x00000008;
constexpr int SCE_NET_EPOLLHUP    = 0x00000010;
constexpr int SCE_NET_EPOLLDESCID = 0x00010000;

constexpr int SCE_NET_EPOLL_CTL_ADD = 1;
constexpr int SCE_NET_EPOLL_CTL_MOD = 2;
constexpr int SCE_NET_EPOLL_CTL_DEL = 3;

constexpr int SCE_NET_EPOLL_ABORT_FLAG_PRESERVATION = 0x00000001;

#define EPOLLDESCID (1u << 14)

static inline uint32_t sce_map_epoll_events(uint32_t events) {
  LOG_USE_MODULE(OnlineNetCore_epoll);
  uint32_t _events = 0;
  if (events & SCE_NET_EPOLLIN) _events |= EPOLLIN;
  if (events & SCE_NET_EPOLLOUT) _events |= EPOLLOUT;
  if (events & SCE_NET_EPOLLERR) _events |= EPOLLERR;
  if (events & SCE_NET_EPOLLHUP) _events |= EPOLLHUP;
  if (events & SCE_NET_EPOLLDESCID) _events |= EPOLLDESCID; // Not an actual event, todo
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

SceNetId OnlineNet::epollCreate(const char* name, int flags) {
  static SceNetId id = 0;
  HANDLE          ep;

  if ((ep = epoll_create1(flags)) != NULL && ep != INVALID_HANDLE_VALUE) {
    map[++id] = ep;
    return id;
  }

  return INetworking::getLastError();
}

int OnlineNet::epollControl(SceNetId eid, int op, SceNetId id, SceNetEpollEvent* event) {
  epoll_event ev {
      .events = sce_map_epoll_events(event->events),
      .data   = {.ptr = event->data.ptr},
  };

  if (ev.events & EPOLLDESCID) return Ok; // todo handle all events
  if (epoll_ctl(sce_map_id_get(eid), op, id, &ev) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::epollWait(SceNetId eid, SceNetEpollEvent* events, int maxevents, int timeout) {
  epoll_event ev {
      .events = sce_map_epoll_events(events->events),
      .data   = {.ptr = events->data.ptr},
  };

  if (ev.events & EPOLLDESCID) return Ok; // todo handle all events
  if (epoll_wait(sce_map_id_get(eid), &ev, maxevents, timeout) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::epollDestroy(SceNetId eid) {
  if (epoll_close(sce_map_id_del(eid)) == SOCKET_ERROR) return INetworking::getLastError();
  return Ok;
}

int OnlineNet::epollAbort(SceNetId eid, int flags) {
  return Ok;
}
