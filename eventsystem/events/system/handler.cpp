#include "handler.h"

#include "intern/export.h"

namespace events::system {
IEventHandler::IEventHandler() {
  core::registerSelf(this);
}
} // namespace events::system
