#include "common.h"
#include "logging.h"
#include "types.h"

namespace {} // namespace

extern "C" {
EXPORT const char* MODULE_NAME = "libSceDiscMap";

EXPORT SYSV_ABI int sceDiscMapIsRequestOnHDD(const char* file, uint64_t a2, uint64_t a3, const int* a4) {
  return Ok;
}

EXPORT SYSV_ABI int __NID_HEX(7C980FFB0AA27E7A)() {
  return Ok;
}

EXPORT SYSV_ABI int __NID_HEX(7E5D5EA039F0438B)() {
  return Ok;
}

EXPORT SYSV_ABI int __NID_HEX(8A828CAEE7EDD5E9)() {
  return Ok;
}

EXPORT SYSV_ABI int __NID_HEX(E7EBCE96E92F91F8)() {
  return Ok;
}
}