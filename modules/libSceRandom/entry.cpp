#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceRandom);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libSceRandom";

EXPORT SYSV_ABI int32_t sceRandomGetRandomNumber(void* buf, uint32_t size) {
  LOG_USE_MODULE(libSceRandom);
    if (buf == nullptr || size > Err::RANDOM_MAX_SIZE) {
     return EINVAL;
    }
}
}