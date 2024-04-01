#pragma once

#include "modules_include/common.h"

#include <stdint.h>

namespace filesystem {

enum class SceMapMode : uint16_t {
  SHARED       = 0x0001,
  PRIVATE      = 0x0002,
  FIXED        = 0x0010,
  NO_OVERWRITE = 0x0080,
  VOID_        = 0x0100,
};

enum class SceMapType : uint16_t {
  FILE         = 0x0,
  ANON         = 0x1,
  SYSTEM       = 0x2,
  ALLAVAILABLE = 0x4,
};

struct SceMap {
  SceMapMode mode : 12;
  SceMapType type : 4;
};

enum class SceOpenMode : uint32_t {
  RDONLY = 0,
  WRONLY = 1,
  RDWR   = 2,
};

struct SceOpen {
  SceOpenMode mode      : 2;
  uint32_t    nonblock  : 1;
  uint32_t    append    : 1;
  uint32_t    shlock    : 1;
  uint32_t    exlock    : 1;
  uint32_t    async     : 1;
  uint32_t    fsync     : 1;
  uint32_t    nofollow  : 1;
  uint32_t    create    : 1;
  uint32_t    trunc     : 1;
  uint32_t    excl      : 1;
  uint32_t    dsync     : 1;
  uint32_t    dummy0    : 1;
  uint32_t    fhaslock  : 1;
  uint32_t    noctty    : 1;
  uint32_t    direct    : 1;
  uint32_t    directory : 1;
  uint32_t    exec      : 1;
  uint32_t    tty_init  : 1;
  uint32_t    cloexec   : 1;
};

struct SceKernelStat {
  uint32_t          dev;
  uint32_t          ino;
  uint16_t          mode;
  uint16_t          nlink;
  uint32_t          uid;
  uint32_t          gid;
  uint32_t          rdev;
  SceKernelTimespec aTime;
  SceKernelTimespec mTime;
  SceKernelTimespec cTime;
  int64_t           size;
  int64_t           blocks;
  uint32_t          blksize;
  uint32_t          flags;
  uint32_t          gen;
  int32_t           lspare;
  SceKernelTimespec birthtime;
  unsigned int: (8 / 2) * (16 - static_cast<int>(sizeof(SceKernelTimespec)));
  unsigned int: (8 / 2) * (16 - static_cast<int>(sizeof(SceKernelTimespec)));
};

struct Sce_iovec {
  void*  iov_base;
  size_t iov_len;
};

typedef uint16_t SceKernelMode; // todo needed?

typedef struct Sce_iovec SceKernelIovec;

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

__APICALL int     mmap(void* addr, size_t len, int prot, SceMap flags, int fd, int64_t offset, void** res);
__APICALL int     munmap(void* address, size_t len);
__APICALL size_t  read(int handle, void* buf, size_t nbytes);
__APICALL int     ioctl(int handle, int request, SceVariadicList argp);
__APICALL int64_t write(int handle, const void* buf, size_t nbytes);
__APICALL int     open(const char* path, SceOpen flags, SceKernelMode kernelMode);
__APICALL int     close(int handle);
__APICALL int     unlink(const char* path);
__APICALL int     chmod(const char* path, SceKernelMode mode);
__APICALL int     checkReachability(const char* path);
__APICALL void    sync(void);
__APICALL int     fsync(int handle);
__APICALL int     fdatasync(int fd);
__APICALL int     fcntl(int fd, int cmd, SceVariadicList argp);
__APICALL size_t  readv(int handle, const SceKernelIovec* iov, int iovcnt);
__APICALL size_t  writev(int handle, const SceKernelIovec* iov, int iovcnt);
__APICALL int     fchmod(int fd, SceKernelMode mode);
__APICALL int     rename(const char* from, const char* to);
__APICALL int     mkdir(const char* path, SceKernelMode mode);
__APICALL int     rmdir(const char* path);
__APICALL int     utimes(const char* path, const SceKernelTimeval* times);
__APICALL int     stat(const char* path, SceKernelStat* sb);
__APICALL int     fstat(int fd, SceKernelStat* sb);
__APICALL int     futimes(int fd, const SceKernelTimeval* times);
__APICALL int     getdirentries(int fd, char* buf, int nbytes, long* basep);
__APICALL int     getdents(int fd, char* buf, int nbytes);
__APICALL size_t  preadv(int handle, const SceKernelIovec* iov, int iovcnt, int64_t offset);
__APICALL size_t  pwritev(int handle, const SceKernelIovec* iov, int iovcnt, int64_t offset);
__APICALL size_t  pread(int handle, void* buf, size_t nbytes, int64_t offset);
__APICALL size_t  pwrite(int handle, const void* buf, size_t nbytes, int64_t offset);
__APICALL int64_t lseek(int handle, int64_t offset, int whence);
__APICALL int     truncate(const char* path, int64_t length);
__APICALL int     ftruncate(int fd, int64_t length);
__APICALL int     setCompressionAttribute(int fd, int flag);
__APICALL int     lwfsSetAttribute(int fd, int flags);
__APICALL int     lwfsAllocateBlock(int fd, int64_t size);
__APICALL int     lwfsTrimBlock(int fd, int64_t size);
__APICALL int64_t lwfsLseek(int fd, int64_t offset, int whence);
__APICALL size_t  lwfsWrite(int fd, const void* buf, size_t nbytes);

#undef __APICALL
}; // namespace filesystem
