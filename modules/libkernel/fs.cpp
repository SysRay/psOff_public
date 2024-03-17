#include "common.h"
#include "core/kernel/errors.h"
#include "core/kernel/filesystem.h"
#include "logging.h"
#include "types.h"

#include <stdarg.h>

LOG_DEFINE_MODULE(fs);

extern "C" {

EXPORT SYSV_ABI size_t sceKernelRead(int handle, void* buf, size_t nbytes) {
  return filesystem::read(handle, buf, nbytes);
}

EXPORT SYSV_ABI int64_t sceKernelWrite(int handle, const void* buf, size_t nbytes) {
  return filesystem::write(handle, buf, nbytes);
}

EXPORT SYSV_ABI int sceKernelOpen(const char* path, filesystem::SceOpen flags, filesystem::SceKernelMode kernelMode) {
  return filesystem::open(path, flags, kernelMode);
}

EXPORT SYSV_ABI int __NID(_open)(const char* path, filesystem::SceOpen flags, filesystem::SceKernelMode kernelMode) {
  return POSIX_CALL(filesystem::open(path, flags, kernelMode));
}

EXPORT SYSV_ABI int sceKernelClose(int handle) {
  return filesystem::close(handle);
}

EXPORT SYSV_ABI int sceKernelUnlink(const char* path) {
  return filesystem::unlink(path);
}

EXPORT SYSV_ABI int sceKernelChmod(const char* path, filesystem::SceKernelMode mode) {
  return filesystem::chmod(path, mode);
}

EXPORT SYSV_ABI int sceKernelCheckReachability(const char* path) {
  return filesystem::checkReachability(path);
}

EXPORT SYSV_ABI void sceKernelSync(void) {
  return filesystem::sync();
}

EXPORT SYSV_ABI int sceKernelFsync(int handle) {
  return filesystem::fsync(handle);
}

EXPORT SYSV_ABI int sceKernelFdatasync(int fd) {
  return filesystem::fdatasync(fd);
}

EXPORT SYSV_ABI int sceKernelFcntl(int fd, int cmd, ...) {
  LOG_USE_MODULE(fs);
  LOG_ERR(L"todo %S", __FUNCTION__);
  // todo own va_start etc.. for sysv_abi
  // va_list args;
  // va_start(args, cmd);

  // return filesystem::fcntl(fd, cmd, args);
  // va_end(args);
  return Ok;
}

EXPORT SYSV_ABI size_t sceKernelReadv(int handle, const filesystem::SceKernelIovec* iov, int iovcnt) {
  return filesystem::readv(handle, iov, iovcnt);
}

EXPORT SYSV_ABI size_t __NID(_readv)(int handle, const filesystem::SceKernelIovec* iov, int iovcnt) {
  return POSIX_CALL(filesystem::readv(handle, iov, iovcnt));
}

EXPORT SYSV_ABI size_t __NID(_writev)(int handle, const filesystem::SceKernelIovec* iov, int iovcnt) {
  return POSIX_CALL(filesystem::writev(handle, iov, iovcnt));
}

EXPORT SYSV_ABI size_t sceKernelWritev(int handle, const filesystem::SceKernelIovec* iov, int iovcnt) {
  return filesystem::writev(handle, iov, iovcnt);
}

EXPORT SYSV_ABI int sceKernelFchmod(int fd, filesystem::SceKernelMode mode) {
  return filesystem::fchmod(fd, mode);
}

EXPORT SYSV_ABI int sceKernelRename(const char* from, const char* to) {
  return filesystem::rename(from, to);
}

EXPORT SYSV_ABI int sceKernelMkdir(const char* path, filesystem::SceKernelMode mode) {
  return filesystem::mkdir(path, mode);
}

EXPORT SYSV_ABI int sceKernelRmdir(const char* path) {
  return filesystem::rmdir(path);
}

EXPORT SYSV_ABI int sceKernelUtimes(const char* path, const SceKernelTimeval* times) {
  return filesystem::utimes(path, times);
}

EXPORT SYSV_ABI int sceKernelStat(const char* path, filesystem::SceKernelStat* sb) {
  return filesystem::stat(path, sb);
}

EXPORT SYSV_ABI int sceKernelFstat(int fd, filesystem::SceKernelStat* sb) {
  return filesystem::fstat(fd, sb);
}

EXPORT SYSV_ABI int sceKernelFutimes(int fd, const SceKernelTimeval* times) {
  return filesystem::futimes(fd, times);
}

EXPORT SYSV_ABI int sceKernelGetdirentries(int fd, char* buf, int nbytes, long* basep) {
  return filesystem::getdirentries(fd, buf, nbytes, basep);
}

EXPORT SYSV_ABI int sceKernelGetdents(int fd, char* buf, int nbytes) {
  return filesystem::getdirentries(fd, buf, nbytes, nullptr);
}

EXPORT SYSV_ABI size_t sceKernelPreadv(int handle, const filesystem::SceKernelIovec* iov, int iovcnt, int64_t offset) {
  return filesystem::preadv(handle, iov, iovcnt, offset);
}

EXPORT SYSV_ABI size_t sceKernelPwritev(int handle, const filesystem::SceKernelIovec* iov, int iovcnt, int64_t offset) {
  return filesystem::pwritev(handle, iov, iovcnt, offset);
}

EXPORT SYSV_ABI size_t sceKernelPread(int handle, void* buf, size_t nbytes, int64_t offset) {
  return filesystem::pread(handle, buf, nbytes, offset);
}

EXPORT SYSV_ABI int64_t __NID(_read)(int d, void* buf, uint64_t nbytes) {
  return POSIX_CALL(filesystem::read(d, buf, nbytes));
}

EXPORT SYSV_ABI size_t sceKernelPwrite(int handle, const void* buf, size_t nbytes, int64_t offset) {
  return filesystem::pwrite(handle, buf, nbytes, offset);
}

EXPORT SYSV_ABI int __NID(_write)(int handle, const void* buf, size_t nbytes) {
  return POSIX_CALL(filesystem::write(handle, buf, nbytes));
}

EXPORT SYSV_ABI int sceKernelMmap(void* addr, size_t len, int prot, filesystem::SceMap flags, int fd, int64_t offset, void** res) {
  return filesystem::mmap(addr, len, prot, flags, fd, offset, res);
}

EXPORT SYSV_ABI int sceKernelMunmap(void* addr, size_t len) {
  return filesystem::munmap(addr, len);
}

EXPORT SYSV_ABI int64_t sceKernelLseek(int handle, int64_t offset, int whence) {
  return filesystem::lseek(handle, offset, whence);
}

EXPORT SYSV_ABI int sceKernelTruncate(const char* path, int64_t length) {
  return filesystem::truncate(path, length);
}

EXPORT SYSV_ABI int sceKernelFtruncate(int fd, int64_t length) {
  return filesystem::ftruncate(fd, length);
}

EXPORT SYSV_ABI int sceKernelSetCompressionAttribute(int fd, int flag) {
  return filesystem::setCompressionAttribute(fd, flag);
}

EXPORT SYSV_ABI int sceKernelLwfsSetAttribute(int fd, int flags) {
  return filesystem::lwfsSetAttribute(fd, flags);
}

EXPORT SYSV_ABI int sceKernelLwfsAllocateBlock(int fd, int64_t size) {
  return filesystem::lwfsSetAttribute(fd, size);
}

EXPORT SYSV_ABI int sceKernelLwfsTrimBlock(int fd, int64_t size) {
  return filesystem::lwfsTrimBlock(fd, size);
}

EXPORT SYSV_ABI int64_t sceKernelLwfsLseek(int fd, int64_t offset, int whence) {
  return filesystem::lwfsLseek(fd, offset, whence);
}

EXPORT SYSV_ABI size_t sceKernelLwfsWrite(int fd, const void* buf, size_t nbytes) {
  return filesystem::lwfsWrite(fd, buf, nbytes);
}
}
