#pragma once
#include <stdint.h>

constexpr int        Ok = 0;
typedef unsigned int SceKernelUseconds;
typedef void*        SceVariadicList;

struct timeval64 {
  uint64_t tv_sec;
  uint64_t tv_usec;
};

struct timespec64 {
  uint64_t tv_sec;
  uint64_t tv_nsec;
};

typedef struct timeval64  SceKernelTimeval;
typedef struct timespec64 SceKernelTimespec;

struct SceKernelTimezone {
  int tz_minuteswest;
  int tz_dsttime;
};

struct SceRtcTick {
  uint64_t tick;
};

using SceKernelModule      = int;
using SceKernelCpumask     = uint64_t;
using SceUserServiceUserId = int32_t;
using SceKernelClockid     = int32_t;

#define SYSV_ABI __attribute__((sysv_abi))
#define EXPORT   __declspec(dllexport)

enum class ErrCode : int {
  _EPERM   = 1,                     /* Operation not permitted */
  _ENOENT  = 2,                     /* No such file or directory */
  _ESRCH   = 3,                     /* No such process */
  _EINTR   = 4,                     /* Interrupted system call */
  _EIO     = 5,                     /* Input/output error */
  _ENXIO   = 6,                     /* Device not configured */
  _E2BIG   = 7,                     /* Argument list too long */
  _ENOEXEC = 8,                     /* Exec format error */
  _EBADF   = 9,                     /* Bad file descriptor */
  _ECHILD  = 10,                    /* No child processes */
  _EDEADLK = 11,                    /* Resource deadlock avoided */
                                    /*  11 was EAGAIN */
  _ENOMEM            = 12,          /* Cannot allocate memory */
  _EACCES            = 13,          /* Permission denied */
  _EFAULT            = 14,          /* Bad address */
  _ENOTBLK           = 15,          /* Block device required */
  _EBUSY             = 16,          /* Device busy */
  _EEXIST            = 17,          /* File exists */
  _EXDEV             = 18,          /* Cross-device link */
  _ENODEV            = 19,          /* Operation not supported by device */
  _ENOTDIR           = 20,          /* Not a directory */
  _EISDIR            = 21,          /* Is a directory */
  _EINVAL            = 22,          /* Invalid argument */
  _ENFILE            = 23,          /* Too many open files in system */
  _EMFILE            = 24,          /* Too many open files */
  _ENOTTY            = 25,          /* Inappropriate ioctl for device */
  _ETXTBSY           = 26,          /* Text file busy */
  _EFBIG             = 27,          /* File too large */
  _ENOSPC            = 28,          /* No space left on device */
  _ESPIPE            = 29,          /* Illegal seek */
  _EROFS             = 30,          /* Read-only filesystem */
  _EMLINK            = 31,          /* Too many links */
  _EPIPE             = 32,          /* Broken pipe */
  _EDOM              = 33,          /* Numerical argument out of domain */
  _ERANGE            = 34,          /* Result too large */
  _EAGAIN            = 35,          /* Resource temporarily unavailable */
  _EWOULDBLOCK       = _EAGAIN,     /* Operation would block */
  _EINPROGRESS       = 36,          /* Operation now in progress */
  _EALREADY          = 37,          /* Operation already in progress */
  _ENOTSOCK          = 38,          /* Socket operation on non-socket */
  _EDESTADDRREQ      = 39,          /* Destination address required */
  _EMSGSIZE          = 40,          /* Message too long */
  _EPROTOTYPE        = 41,          /* Protocol wrong type for socket */
  _ENOPROTOOPT       = 42,          /* Protocol not available */
  _EPROTONOSUPPORT   = 43,          /* Protocol not supported */
  _ESOCKTNOSUPPORT   = 44,          /* Socket type not supported */
  _EOPNOTSUPP        = 45,          /* Operation not supported */
  _ENOTSUP           = _EOPNOTSUPP, /* Operation not supported */
  _EPFNOSUPPORT      = 46,          /* Protocol family not supported */
  _EAFNOSUPPORT      = 47,          /* Address family not supported by protocol family */
  _EADDRINUSE        = 48,          /* Address already in use */
  _EADDRNOTAVAIL     = 49,          /* Can't assign requested address */
  _ENETDOWN          = 50,          /* Network is down */
  _ENETUNREACH       = 51,          /* Network is unreachable */
  _ENETRESET         = 52,          /* Network dropped connection on reset */
  _ECONNABORTED      = 53,          /* Software caused connection abort */
  _ECONNRESET        = 54,          /* Connection reset by peer */
  _ENOBUFS           = 55,          /* No buffer space available */
  _EISCONN           = 56,          /* Socket is already connected */
  _ENOTCONN          = 57,          /* Socket is not connected */
  _ESHUTDOWN         = 58,          /* Can't send after socket shutdown */
  _ETOOMANYREFS      = 59,          /* Too many references: can't splice */
  _ETIMEDOUT         = 60,          /* Operation timed out */
  _ECONNREFUSED      = 61,          /* Connection refused */
  _ELOOP             = 62,          /* Too many levels of symbolic links */
  _ENAMETOOLONG      = 63,          /* File name too long */
  _EHOSTDOWN         = 64,          /* Host is down */
  _EHOSTUNREACH      = 65,          /* No route to host */
  _ENOTEMPTY         = 66,          /* Directory not empty */
  _EPROCLIM          = 67,          /* Too many processes */
  _EUSERS            = 68,          /* Too many users */
  _EDQUOT            = 69,          /* Disc quota exceeded */
  _ESTALE            = 70,          /* Stale NFS file handle */
  _EREMOTE           = 71,          /* Too many levels of remote in path */
  _EBADRPC           = 72,          /* RPC struct is bad */
  _ERPCMISMATCH      = 73,          /* RPC version wrong */
  _EPROGUNAVAIL      = 74,          /* RPC prog. not avail */
  _EPROGMISMATCH     = 75,          /* Program version wrong */
  _EPROCUNAVAIL      = 76,          /* Bad procedure for program */
  _ENOLCK            = 77,          /* No locks available */
  _ENOSYS            = 78,          /* Function not implemented */
  _EFTYPE            = 79,          /* Inappropriate file type or format */
  _EAUTH             = 80,          /* Authentication error */
  _ENEEDAUTH         = 81,          /* Need authenticator */
  _EIDRM             = 82,          /* Identifier removed */
  _ENOMSG            = 83,          /* No message of desired type */
  _EOVERFLOW         = 84,          /* Value too large to be stored in data type */
  _ECANCELED         = 85,          /* Operation canceled */
  _EILSEQ            = 86,          /* Illegal byte sequence */
  _ENOATTR           = 87,          /* Attribute not found */
  _EDOOFUS           = 88,          /* Programming error */
  _EBADMSG           = 89,          /* Bad message */
  _EMULTIHOP         = 90,          /* Multihop attempted */
  _ENOLINK           = 91,          /* Link has been severed */
  _EPROTO            = 92,          /* Protocol error */
  _ENOTCAPABLE       = 93,          /* Capabilities insufficient */
  _ECAPMODE          = 94,          /* Not permitted in capability mode */
  _ENOBLK            = 95,
  _EICV              = 96,
  _ENOPLAYGOENT      = 97,
  _EREVOKE           = 98,
  _ESDKVERSION       = 99,
  _ESTART            = 100, /* module_start() fails */
  _ESTOP             = 101, /* module_stop() fails */
  _EINVALID2MB       = 102, /* 2MB page is enabled against old binary */
  _ELAST             = 102, /* Must be equal largest errno */
  _EADHOC            = 160, /* adhoc mode */
  _EINACTIVEDISABLED = 163, /* IP address was changed */
  _ENETNODATA        = 164, /* internal code */
  _ENETDESC          = 165, /* internal code */
  _ENETDESCTIMEDOUT  = 166, /* internal code */
  _ENETINTR          = 167, /* network abort */
  _ERETURN           = 205, /* libnetctl error */
};

constexpr int getErr(ErrCode err) {
  if ((int)err == Ok) return 0;

  return (int)err == Ok ? 0 : (int32_t)(0x80020000 + (int32_t)err);
}

#define __NID_HEX_(hexId, funcName) __hex_##hexId
#define __NID_HEX(hexId)            __hex_##hexId
#define __NID(func)                 __sce_##func
#define __DUMMY(func)               __sce_##func

static void ns2timespec(SceKernelTimespec* ts, uint64_t const ns) {
  ts->tv_sec  = (decltype(ts->tv_sec))(ns / 1000000000l);
  ts->tv_nsec = (decltype(ts->tv_nsec))(ns % 1000000000l);
}

static void micro2timeval(SceKernelTimeval* tp, uint64_t const us) {
  tp->tv_sec  = static_cast<decltype(tp->tv_sec)>(us / 1000000UL);
  tp->tv_usec = static_cast<decltype(tp->tv_usec)>(us % 1000000UL);
}
