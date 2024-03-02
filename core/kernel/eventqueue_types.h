#pragma once
#include <stdint.h>

namespace Kernel::EventQueue {
constexpr int16_t KERNEL_EVFILT_TIMER     = -7;
constexpr int16_t KERNEL_EVFILT_READ      = -1;
constexpr int16_t KERNEL_EVFILT_WRITE     = -2;
constexpr int16_t KERNEL_EVFILT_USER      = -11;
constexpr int16_t KERNEL_EVFILT_FILE      = -4;
constexpr int16_t KERNEL_EVFILT_GRAPHICS  = -14;
constexpr int16_t KERNEL_EVFILT_VIDEO_OUT = -13;
constexpr int16_t KERNEL_EVFILT_HRTIMER   = -15;

enum class EventFlags : uint32_t {
  EV_NONE     = 0,
  EV_ONESHOT  = 0x10, // only report one occurrence
  EV_CLEAR    = 0x20, // clear event state after reporting
  EV_RECEIPT  = 0x40, // force EV_ERROR on success, data=0
  EV_DISPATCH = 0x80, // disable event after reporting

  EV_SYSFLAGS = 0xF000, // reserved by system
  EV_FLAG1    = 0x2000, // filter-specific flag
};

struct KernelEvent {
  int        ident  = 0;
  int16_t    filter = 0;
  EventFlags flags  = EventFlags::EV_NONE;
  uint32_t   fflags = 0;
  intptr_t   data   = 0;
  void*      udata  = nullptr;
};
struct KernelEqueueEvent;
class IKernelEqueue;
using IKernelEqueue_t = IKernelEqueue*;
using KernelEvent_t   = KernelEvent*;

using trigger_func_t = void (*)(KernelEqueueEvent* event, void* trigger_data);
using reset_func_t   = void (*)(KernelEqueueEvent* event);
using delete_func_t  = void (*)(IKernelEqueue_t eq, KernelEqueueEvent* event);

struct KernelFilter {
  void*          data              = nullptr;
  trigger_func_t trigger_func      = nullptr;
  reset_func_t   reset_func        = nullptr;
  delete_func_t  delete_event_func = nullptr;
};

struct KernelEqueueEvent {
  bool         triggered = false;
  KernelEvent  event;
  KernelFilter filter;
};

} // namespace Kernel::EventQueue