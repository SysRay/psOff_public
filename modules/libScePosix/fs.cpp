#include "common.h"
#include "core/kernel/errors.h"
#include "core/kernel/filesystem.h"
#include "logging.h"
#include "types.h"

#include <stdarg.h>

LOG_DEFINE_MODULE(fs_posix);

extern "C" {

EXPORT SYSV_ABI size_t __NID(read)(int handle, void* buf, size_t nbytes) {
  return POSIX_CALL(filesystem::read(handle, buf, nbytes));
}

EXPORT SYSV_ABI int64_t __NID(write)(int handle, const void* buf, size_t nbytes) {
  return POSIX_CALL(filesystem::write(handle, buf, nbytes));
}

EXPORT SYSV_ABI int __NID(open)(const char* path, filesystem::SceOpen flags, filesystem::SceKernelMode kernelMode) {
  return POSIX_CALL(filesystem::open(path, flags, kernelMode));
}

EXPORT SYSV_ABI int __NID(close)(int handle) {
  return POSIX_CALL(filesystem::close(handle));
}

EXPORT SYSV_ABI int __NID(unlink)(const char* path) {
  return POSIX_CALL(filesystem::unlink(path));
}

EXPORT SYSV_ABI int __NID(chmod)(const char* path, filesystem::SceKernelMode mode) {
  return POSIX_CALL(filesystem::chmod(path, mode));
}

EXPORT SYSV_ABI void __NID(sync)(void) {
  filesystem::sync();
}

EXPORT SYSV_ABI int __NID(fsync)(int handle) {
  return POSIX_CALL(filesystem::fsync(handle));
}

EXPORT SYSV_ABI int __NID(fcntl)(int fd, int cmd, ...) {
  LOG_USE_MODULE(fs_posix);
  LOG_ERR(L"todo %S", __FUNCTION__);
  // todo own va_start etc.. for sysv_abi
  // va_list args;
  // va_start(args, cmd);

  // return filesystem::fcntl(fd, cmd, args);
  // va_end(args);
  return Ok;
}

EXPORT SYSV_ABI size_t __NID(readv)(int handle, const filesystem::SceKernelIovec* iov, int iovcnt) {
  return POSIX_CALL(filesystem::readv(handle, iov, iovcnt));
}

EXPORT SYSV_ABI size_t __NID(writev)(int handle, const filesystem::SceKernelIovec* iov, int iovcnt) {
  return POSIX_CALL(filesystem::writev(handle, iov, iovcnt));
}

EXPORT SYSV_ABI int __NID(fchmod)(int fd, filesystem::SceKernelMode mode) {
  return POSIX_CALL(filesystem::fchmod(fd, mode));
}

EXPORT SYSV_ABI int __NID(rename)(const char* from, const char* to) {
  return POSIX_CALL(filesystem::rename(from, to));
}

EXPORT SYSV_ABI int __NID(mkdir)(const char* path, filesystem::SceKernelMode mode) {
  return POSIX_CALL(filesystem::mkdir(path, mode));
}

EXPORT SYSV_ABI int __NID(rmdir)(const char* path) {
  return POSIX_CALL(filesystem::rmdir(path));
}

EXPORT SYSV_ABI int __NID(utimes)(const char* path, const SceKernelTimeval* times) {
  return POSIX_CALL(filesystem::utimes(path, times));
}

EXPORT SYSV_ABI int __NID(stat)(const char* path, filesystem::SceKernelStat* sb) {
  return POSIX_CALL(filesystem::stat(path, sb));
}

EXPORT SYSV_ABI int __NID(fstat)(int fd, filesystem::SceKernelStat* sb) {
  return POSIX_CALL(filesystem::fstat(fd, sb));
}

EXPORT SYSV_ABI int __NID(futimes)(int fd, const SceKernelTimeval* times) {
  return POSIX_CALL(filesystem::futimes(fd, times));
}

EXPORT SYSV_ABI int __NID(getdirentries)(int fd, char* buf, int nbytes, long* basep) {
  return POSIX_CALL(filesystem::getdirentries(fd, buf, nbytes, basep));
}

EXPORT SYSV_ABI int __NID(getdents)(int fd, char* buf, int nbytes) {
  return POSIX_CALL(filesystem::getdirentries(fd, buf, nbytes, nullptr));
}

EXPORT SYSV_ABI size_t __NID(preadv)(int handle, const filesystem::SceKernelIovec* iov, int iovcnt, int64_t offset) {
  return POSIX_CALL(filesystem::preadv(handle, iov, iovcnt, offset));
}

EXPORT SYSV_ABI size_t __NID(pwritev)(int handle, const filesystem::SceKernelIovec* iov, int iovcnt, int64_t offset) {
  return POSIX_CALL(filesystem::pwritev(handle, iov, iovcnt, offset));
}

EXPORT SYSV_ABI size_t __NID(pread)(int handle, void* buf, size_t nbytes, int64_t offset) {
  return POSIX_CALL(filesystem::pread(handle, buf, nbytes, offset));
}

EXPORT SYSV_ABI size_t __NID(pwrite)(int handle, const void* buf, size_t nbytes, int64_t offset) {
  return POSIX_CALL(filesystem::pwrite(handle, buf, nbytes, offset));
}

EXPORT SYSV_ABI int64_t __NID(lseek)(int handle, int64_t offset, int whence) {
  return POSIX_CALL(filesystem::lseek(handle, offset, whence));
}

EXPORT SYSV_ABI int __NID(truncate)(const char* path, int64_t length) {
  return POSIX_CALL(filesystem::truncate(path, length));
}

EXPORT SYSV_ABI int __NID(ftruncate)(int fd, int64_t length) {
  return POSIX_CALL(filesystem::ftruncate(fd, length));
}
}