#include "common.h"
#include "logging.h"
#include "types.h"

LOG_DEFINE_MODULE(libSceLibcInternalExt);

namespace {
struct HeapInfoInfo {
  uint64_t  size = sizeof(HeapInfoInfo);
  uint32_t  flag;
  uint32_t  getSegmentInfo;
  uint64_t* mspace_atomic_id_mask;
  uint64_t* mstate_table;
};

} // namespace

extern "C" {

const char* MODULE_NAME = "libSceLibcInternal";

EXPORT SYSV_ABI void sceLibcHeapGetTraceInfo(HeapInfoInfo* info) {
  static uint64_t g_mspace_atomic_id_mask = 0;
  static uint64_t g_mstate_table[64]      = {0};

  info->mspace_atomic_id_mask = &g_mspace_atomic_id_mask;
  info->mstate_table          = g_mstate_table;
  info->getSegmentInfo        = 0;
}
}