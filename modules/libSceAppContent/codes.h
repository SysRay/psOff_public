#pragma once
#include <stdint.h>

namespace Err {
constexpr int32_t NOT_INITIALIZED       = -2133262335; /* 0x80D90001 */
constexpr int32_t PARAMETER             = -2133262334; /* 0x80D90002 */
constexpr int32_t BUSY                  = -2133262333; /* 0x80D90003 */
constexpr int32_t NOT_MOUNTED           = -2133262332; /* 0x80D90004 */
constexpr int32_t NOT_FOUND             = -2133262331; /* 0x80D90005 */
constexpr int32_t MOUNT_FULL            = -2133262330; /* 0x80D90006 */
constexpr int32_t DRM_NO_ENTITLEMENT    = -2133262329; /* 0x80D90007 */
constexpr int32_t NO_SPACE              = -2133262328; /* 0x80D90008 */
constexpr int32_t NOT_SUPPORTED         = -2133262327; /* 0x80D90009 */
constexpr int32_t INTERNAL              = -2133262326; /* 0x80D9000A */
constexpr int32_t DOWNLOAD_ENTRY_FULL   = -2133262325; /* 0x80D9000B */
constexpr int32_t INVALID_PKG           = -2133262324; /* 0x80D9000C */
constexpr int32_t OTHER_APPLICATION_PKG = -2133262323; /* 0x80D9000D */
constexpr int32_t CREATE_FULL           = -2133262322; /* 0x80D9000E */
constexpr int32_t MOUNT_OTHER_APP       = -2133262321; /* 0x80D9000F */
constexpr int32_t OF_MEMORY             = -2133262320; /* 0x80D90010 */
constexpr int32_t ADDCONT_SHRANK        = -2133262319; /* 0x80D90011 */
constexpr int32_t ADDCONT_NO_IN_QUEUE   = -2133262318; /* 0x80D90012 */
constexpr int32_t NETWORK               = -2133262317; /* 0x80D90013 */
constexpr int32_t SIGNED_OUT            = -2133262316; /* 0x80D90014 */
} // namespace Err

constexpr uint32_t MOUNTPOINT_DATA_MAXSIZE = 16;
constexpr uint32_t ADDCONT_MOUNT_MAXNUM    = 64;
constexpr uint32_t ENTITLEMENT_KEY_SIZE    = 16;
constexpr uint32_t INFO_LIST_MAX_SIZE      = 2500;
