#include "handler.h"

#include "intern/export.h"

namespace events::system_circle {
IEventHandler::IEventHandler() {
  core::registerSelf(this);
}
} // namespace events::system_circle
