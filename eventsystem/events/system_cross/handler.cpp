#include "handler.h"

#include "intern/export.h"

namespace events::system_cross {
IEventHandler::IEventHandler() {
  core::registerSelf(this);
}
} // namespace events::system_cross
