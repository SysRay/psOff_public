#define __APICALL_EXTERN
#include "eventqueue.h"
#undef __APICALL_EXTERN

#include "logging.h"
#include "modules_include/common.h"

#include <algorithm>
#include <boost/chrono.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <chrono>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Kernel {
namespace EventQueue {
LOG_DEFINE_MODULE(EventQueue);

class KernelEqueue: public IKernelEqueue {
  public:
  KernelEqueue() = default;
  virtual ~KernelEqueue();

  [[nodiscard]] std::string const& getName() const { return m_name; }

  void setName(std::string&& name) { m_name.swap(name); }

  int waitForEvents(KernelEvent_t ev, int num, SceKernelUseconds const* micros);

  int pollEvents(KernelEvent_t ev, int num) {
    std::unique_lock lock(m_mutex_cond);
    auto             ret = getTriggeredEvents(ev, num);

    return ret;
  }

  int wait(KernelEvent_t ev, int num, int* out, const SceKernelUseconds* time) final;

  int addUserEvent(int ident) final;
  int addUserEventEdge(int ident) final;
  int triggerUserEvent(uintptr_t ident, void* udata) final;

  int addEvent(const KernelEqueueEvent& event) final;
  int triggerEvent(uintptr_t ident, int16_t filter, void* trigger_data) final;
  int deleteEvent(uintptr_t ident, int16_t filter) final;

  private:
  int getTriggeredEvents(KernelEvent_t ev, int num);

  std::list<KernelEqueueEvent> m_events; // We dont ecpext many events ~5

  boost::mutex              m_mutex_cond;
  std::mutex                m_mutex_int;
  boost::condition_variable m_cond_var;

  std::string m_name;
  bool        m_closed = false;
};

KernelEqueue::~KernelEqueue() {
  std::unique_lock const lock(m_mutex_cond);
  /*
  for (auto& ev: m_events) {
      if (ev.filter.delete_event_func != nullptr) {
          ev.filter.delete_event_func(this, &ev);
      }
  }*/
  m_closed = true;
  m_cond_var.notify_all();
}

int KernelEqueue::waitForEvents(KernelEvent_t ev, int num, SceKernelUseconds const* micros) {
  LOG_USE_MODULE(EventQueue);
  // LOG_TRACE(L"->waitForEvents: ident:0x%08llx num:%d", ev->ident, num);

  boost::unique_lock lock(m_mutex_cond);

  int ret = 0;
  if (micros != nullptr && *micros > 0) {
    using namespace std::chrono_literals;

    if (m_cond_var.wait_for(lock, boost::chrono::microseconds(*(decltype(micros))micros), [&] {
          ret = getTriggeredEvents(ev, num);
          return (ret > 0) | m_closed;
        }) == 0) {
      // LOG_TRACE(L"waitForEvents timeout");
    }
  } else {
    m_cond_var.wait(lock, [&] {
      ret = getTriggeredEvents(ev, num);
      return (ret > 0) | m_closed;
    });
  }
  // LOG_TRACE(L"<-waitForEvents: ident:0x%08llx ret:%d", ev->ident, ret);
  return ret;
}

int KernelEqueue::getTriggeredEvents(KernelEvent_t eventList, int num) {
  LOG_USE_MODULE(EventQueue);

  int countTriggered = 0;
  for (auto& ev: m_events) {
    if (ev.triggered) {
      eventList[countTriggered++] = ev.event;

      if ((((uint32_t)ev.event.flags & (uint32_t)EventFlags::EV_CLEAR) != 0) && ev.filter.reset_func != nullptr) {
        ev.filter.reset_func(&ev);
      }
      LOG_TRACE(L"Event Triggered: ident:0x%08llx, filter:%d", (uint64_t)ev.event.ident, ev.event.filter);
      if (countTriggered >= num) {
        break;
      }
    }
  }

  return countTriggered;
}

void event_trigger_func(Kernel::EventQueue::KernelEqueueEvent* event, void* trigger_data) {
  event->triggered = true;
  event->event.fflags++;
  event->event.udata = trigger_data;
}

void event_reset_func(Kernel::EventQueue::KernelEqueueEvent* event) {
  event->triggered    = false;
  event->event.fflags = 0;
  event->event.udata  = 0;
}

void event_delete_func(Kernel::EventQueue::IKernelEqueue_t eq, Kernel::EventQueue::KernelEqueueEvent* event) {}

int createEqueue(IKernelEqueue_t* eq, const char* name) {
  LOG_USE_MODULE(EventQueue);
  if (eq == nullptr || name == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto item = std::make_unique<KernelEqueue>();
  item->setName(std::string(name));

  LOG_INFO(L"+EventQueue (%S)", item->getName().c_str());
  *eq = item.release();
  return Ok;
}

int deleteEqueue(IKernelEqueue_t eq) {
  LOG_USE_MODULE(EventQueue);

  if (eq == nullptr) {
    return getErr(ErrCode::_EBADF);
  }

  LOG_INFO(L"-EventQueue (%S)", ((KernelEqueue*)eq)->getName().c_str());
  delete (eq);

  return Ok;
}

int KernelEqueue::wait(KernelEvent_t ev, int num, int* out, const SceKernelUseconds* time) {
  if (ev == nullptr) {
    return getErr(ErrCode::_EFAULT);
  }

  if (num < 1) {
    return getErr(ErrCode::_EINVAL);
  }

  if (time == nullptr) {
    *out = waitForEvents(ev, num, nullptr);
    if (*out == 0) {
      return getErr(ErrCode::_ETIMEDOUT);
    }
  } else {
    *out = waitForEvents(ev, num, time);
    if (*out == 0 && *time > 0) {
      return getErr(ErrCode::_ETIMEDOUT);
    }
  }

  return Ok;
}

int KernelEqueue::addUserEvent(int ident) {
  return addEvent(Kernel::EventQueue::KernelEqueueEvent {.triggered = false,
                                                         .event =
                                                             {
                                                                 .ident  = ident,
                                                                 .filter = Kernel::EventQueue::KERNEL_EVFILT_USER,
                                                                 .flags  = Kernel::EventQueue::EventFlags::EV_NONE,
                                                                 .fflags = 0,
                                                                 .data   = 0,
                                                                 .udata  = 0,

                                                             },
                                                         .filter {
                                                             .data              = nullptr,
                                                             .trigger_func      = event_trigger_func,
                                                             .reset_func        = event_reset_func,
                                                             .delete_event_func = event_delete_func,
                                                         }});
}

int KernelEqueue::addUserEventEdge(int ident) {
  return addEvent(Kernel::EventQueue::KernelEqueueEvent {.triggered = false,
                                                         .event =
                                                             {
                                                                 .ident  = ident,
                                                                 .filter = Kernel::EventQueue::KERNEL_EVFILT_USER,
                                                                 .flags  = Kernel::EventQueue::EventFlags::EV_CLEAR,
                                                                 .fflags = 0,
                                                                 .data   = 0,
                                                                 .udata  = 0,

                                                             },
                                                         .filter {
                                                             .data              = nullptr,
                                                             .trigger_func      = event_trigger_func,
                                                             .reset_func        = event_reset_func,
                                                             .delete_event_func = event_delete_func,
                                                         }});
}

int KernelEqueue::triggerUserEvent(uintptr_t ident, void* udata) {
  LOG_USE_MODULE(EventQueue);
  LOG_TRACE(L"triggerUserEvent: ident:0x%08llx", (uint64_t)ident);

  return triggerEvent(ident, Kernel::EventQueue::KERNEL_EVFILT_USER, udata);
}

int KernelEqueue::addEvent(const KernelEqueueEvent& event) {
  LOG_USE_MODULE(EventQueue);
  LOG_INFO(L"(%S) Add Event: ident:0x%08llx, filter:%d", m_name.c_str(), (uint64_t)event.event.ident, event.event.filter);

  std::unique_lock const lock(m_mutex_cond);

  auto it = std::find_if(m_events.begin(), m_events.end(),
                         [=](KernelEqueueEvent const& ev) { return ev.event.ident == event.event.ident && ev.event.filter == event.event.filter; });

  if (it != m_events.end()) {
    *it = event;
  } else {
    m_events.push_back(event);
  }

  if (event.triggered) {
    m_cond_var.notify_all();
  }

  return Ok;
}

int KernelEqueue::triggerEvent(uintptr_t ident, int16_t filter, void* trigger_data) {
  LOG_USE_MODULE(EventQueue);
  LOG_TRACE(L"triggerEvent: ident:0x%08llx, filter:%d", (uint64_t)ident, filter);

  std::unique_lock const lock(m_mutex_cond);
  auto it = std::find_if(m_events.begin(), m_events.end(), [=](KernelEqueueEvent const& ev) { return ev.event.ident == ident && ev.event.filter == filter; });

  if (it != m_events.end()) {
    if (it->filter.trigger_func != nullptr) {
      it->filter.trigger_func(&(*it), trigger_data);
    } else {
      it->triggered = true;
    }
    m_cond_var.notify_all();
    return Ok;
  }

  return getErr(ErrCode::_ENOENT);
}

int KernelEqueue::deleteEvent(uintptr_t ident, int16_t filter) {
  LOG_USE_MODULE(EventQueue);
  LOG_INFO(L"deleteEvent: ident:0x%08llx, filter:%llu", (uint64_t)ident, filter);

  std::unique_lock const lock(m_mutex_cond);
  auto it = std::find_if(m_events.begin(), m_events.end(), [=](KernelEqueueEvent const& ev) { return ev.event.ident == ident && ev.event.filter == filter; });

  if (it != m_events.end()) {
    if (it->filter.delete_event_func != nullptr) {
      it->filter.delete_event_func(this, &(*it));
    }

    m_events.erase(it);
    return Ok;
  }

  return getErr(ErrCode::_ENOENT);
}
} // namespace EventQueue
} // namespace Kernel