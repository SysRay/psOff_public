#include "events.h"

#include "intern/export.h"

namespace events::video {
void postEventFlip() {
  core::postEvent(0, nullptr);
}

void postEventVBlank(VBlankData const& data) {
  core::postEvent(1, (void const*)&data);
}
} // namespace events::video