#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libkernel_cpumode_platform);

namespace {} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libkernel";

EXPORT SYSV_ABI int32_t sceKernelIsProspero() {
  return 0; // PS5
}
}
