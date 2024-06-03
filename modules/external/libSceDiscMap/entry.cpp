#include "common.h"
#include "logging.h"
#include "types.h"

namespace {} // namespace

extern "C" {
EXPORT const char* MODULE_NAME = "libSceDiscMap";

EXPORT SYSV_ABI int sceDiscMapIsRequestOnHDD(const char* file, uint64_t a2, uint64_t a3, int* retVal) {
  return Err::DiscMap::NO_BITMAP_INFO;
}

EXPORT SYSV_ABI int __NID_HEX(7C980FFB0AA27E7A)() {
  return Ok;
}

EXPORT SYSV_ABI int sceDiscMapGetPackageSize(int64_t fflags, void* p1, void* p2) {
  return Err::DiscMap::NO_BITMAP_INFO;
}

EXPORT SYSV_ABI int __NID_HEX(8A828CAEE7EDD5E9)(const char* file, void* p1, void* p2, int64_t* pFlags, int64_t* p3, int64_t* p4) {
  return Err::DiscMap::NO_BITMAP_INFO;
}

EXPORT SYSV_ABI int __NID_HEX(E7EBCE96E92F91F8)() {
  return Ok;
}
}
