#include "events.h"

#include "intern/export.h"

namespace events::system {
void postEventLoadExec(LoadExecData const& data) {
  core::postEvent(0, (void const*)&data);
}
} // namespace events::system
