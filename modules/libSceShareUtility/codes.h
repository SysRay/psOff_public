#pragma once
#include <stdint.h>

namespace Err {} // namespace Err

namespace ShareUtility {
constexpr int32_t HEAP_SIZE          = 131072;
constexpr int32_t UPLOAD_CONTENT_MAX = 8192;
} // namespace ShareUtility

enum SceContentSearchMimeType {
  UNKNOWN = 0,
  JPEG    = 1,
  MP4     = 2,
  PNG     = 3,
  GIF     = 4,
};
