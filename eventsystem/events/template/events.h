#pragma once
#include "types.h"

namespace events::video {
void postEventFlip();
void postEventVBlank(VBlankData const& data);
} // namespace events::video