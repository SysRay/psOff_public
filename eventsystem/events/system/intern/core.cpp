#define __APICALL_EXTERN
#include "export.h"
#undef __APICALL_EXTERN

#include "../handler.h"

#include <boost/bind/bind.hpp>
#include <boost/signals2.hpp>

namespace {
auto getSignal() {
  static boost::signals2::signal<void(int, void const*)> signal;
  return &signal;
}

void onEvent(events::system::IEventHandler* handler, int eventId, void const* data) {
  switch (eventId) {
    case 0: handler->onLoadExec(*(events::system::LoadExecData const*)data); return;
  }
}
} // namespace

namespace events::system::core {
void registerSelf(events::system::IEventHandler* obj) {
  using namespace boost::placeholders;
  getSignal()->connect(boost::bind(onEvent, obj, _1, _2));
}

void postEvent(int eventId, void const* data) {
  (*getSignal())(eventId, data);
}
} // namespace events::system::core
