#include "handler.h"

#include "intern/export.h"

namespace events::video {
IEventHandler::IEventHandler() {
  core::registerSelf(this);
}
} // namespace events::video