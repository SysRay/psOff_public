#include "common.h"
#include "core/kernel/errors.h"
#include "core/kernel/filesystem.h"
#include "core/memory/memory.h"
#include "logging.h"
#include "types.h"

#include <stdarg.h>

LOG_DEFINE_MODULE(mman_posix);

extern "C" {

EXPORT SYSV_ABI int sceKernelMmap(void* addr, size_t len, int prot, filesystem::SceMap flags, int fd, int64_t offset, void** res) {
  return filesystem::mmap(addr, len, prot, flags, fd, offset, res);
}

EXPORT SYSV_ABI int sceKernelMunmap(void* addr, size_t len) {
  return filesystem::munmap(addr, len);
}

EXPORT SYSV_ABI void* __NID(mmap)(void* addr, size_t len, int prot, filesystem::SceMap flags, int fd, int64_t offset) {
  void* res;
  auto  tmp = POSIX_CALL(filesystem::mmap(addr, len, prot, flags, fd, offset, &res));

  return res;
}

EXPORT SYSV_ABI int __NID(munmap)(void* addr, size_t len) {
  return POSIX_CALL(filesystem::munmap(addr, len));
}

EXPORT SYSV_ABI int __NID(mlock)(const void* addr, size_t len) {
  LOG_USE_MODULE(mman_posix);
  LOG_DEBUG(L"MLock addr:0x%08llx len:0x%08llx", (uint64_t)addr, len);
  return Ok;
}

EXPORT SYSV_ABI int __NID(munlock)(const void* addr, size_t len) {
  LOG_USE_MODULE(mman_posix);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int __NID(mprotect)(uint64_t addr, size_t len, int prot) {
  if (memory::protect(addr, len, prot, nullptr)) return Ok;
  setError_pthread((int)ErrCode::_EACCES);
  return -1;
}

EXPORT SYSV_ABI int __NID(msync)(void* addr, size_t len, int flags) {
  LOG_USE_MODULE(mman_posix);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

EXPORT SYSV_ABI int __NID(mlockall)(int flags) {
  LOG_USE_MODULE(mman_posix);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}
}
