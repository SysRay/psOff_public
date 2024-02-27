#pragma once
#include <stdint.h>
#include <string_view>

namespace Err {

constexpr int SAVE_DATA_ERROR_PARAMETER                            = -2137063424; /* 0x809F0000 */
constexpr int SAVE_DATA_ERROR_NOT_INITIALIZED                      = -2137063423; /* 0x809F0001 */
constexpr int SAVE_DATA_ERROR_OUT_OF_MEMORY                        = -2137063422; /* 0x809F0002 */
constexpr int SAVE_DATA_ERROR_BUSY                                 = -2137063421; /* 0x809F0003 */
constexpr int SAVE_DATA_ERROR_NOT_MOUNTED                          = -2137063420; /* 0x809F0004 */
constexpr int SAVE_DATA_ERROR_NO_PERMISSION                        = -2137063419; /* 0x809F0005 */
constexpr int SAVE_DATA_ERROR_FINGERPRINT_MISMATCH                 = -2137063418; /* 0x809F0006 */
constexpr int SAVE_DATA_ERROR_EXISTS                               = -2137063417; /* 0x809F0007 */
constexpr int SAVE_DATA_ERROR_NOT_FOUND                            = -2137063416; /* 0x809F0008 */
constexpr int SAVE_DATA_ERROR_NO_SPACE_FS                          = -2137063414; /* 0x809F000A */
constexpr int SAVE_DATA_ERROR_INTERNAL                             = -2137063413; /* 0x809F000B */
constexpr int SAVE_DATA_ERROR_MOUNT_FULL                           = -2137063412; /* 0x809F000C */
constexpr int SAVE_DATA_ERROR_BAD_MOUNTED                          = -2137063411; /* 0x809F000D */
constexpr int SAVE_DATA_ERROR_FILE_NOT_FOUND                       = -2137063410; /* 0x809F000E */
constexpr int SAVE_DATA_ERROR_BROKEN                               = -2137063409; /* 0x809F000F */
constexpr int SAVE_DATA_ERROR_INVALID_LOGIN_USER                   = -2137063407; /* 0x809F0011 */
constexpr int SAVE_DATA_ERROR_MEMORY_NOT_READY                     = -2137063406; /* 0x809F0012 */
constexpr int SAVE_DATA_ERROR_BACKUP_BUSY                          = -2137063405; /* 0x809F0013 */
constexpr int SAVE_DATA_ERROR_NOT_REGIST_CALLBACK                  = -2137063403; /* 0x809F0015 */
constexpr int SAVE_DATA_ERROR_BUSY_FOR_SAVING                      = -2137063402; /* 0x809F0016 */
constexpr int SAVE_DATA_ERROR_LIMITATION_OVER                      = -2137063401; /* 0x809F0017 */
constexpr int SAVE_DATA_ERROR_EVENT_BUSY                           = -2137063400; /* 0x809F0018 */
constexpr int SAVE_DATA_ERROR_PARAMSFO_TRANSFER_TITLE_ID_NOT_FOUND = -2137063399; /* 0x809F0019 */

} // namespace Err

constexpr uint32_t SCE_SAVE_DATA_TITLE_ID_DATA_SIZE       = 10;
constexpr uint32_t SCE_SAVE_DATA_DIRNAME_DATA_MAXSIZE     = 32;
constexpr uint32_t SCE_SAVE_DATA_FINGERPRINT_DATA_SIZE    = 65;
constexpr uint32_t SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE = 16;
constexpr uint32_t SCE_SAVE_DATA_DIRNAME_MAX_COUNT        = 1024;
constexpr uint32_t SCE_SAVE_DATA_MOUNT_MAX_COUNT          = 16;
constexpr uint32_t SCE_SAVE_DATA_TITLE_MAXSIZE            = 128;
constexpr uint32_t SCE_SAVE_DATA_SUBTITLE_MAXSIZE         = 128;
constexpr uint32_t SCE_SAVE_DATA_DETAIL_MAXSIZE           = 1024;

constexpr uint32_t SCE_SAVE_ICON_WIDTH        = 228;
constexpr uint32_t SCE_SAVE_ICON_HEIGHT       = 128;
constexpr uint32_t SCE_SAVE_ICON_FILE_MAXSIZE = SCE_SAVE_ICON_WIDTH * SCE_SAVE_ICON_HEIGHT * 4;

typedef uint64_t   SceSaveDataBlocks;
constexpr uint32_t SCE_SAVE_DATA_BLOCK_SIZE               = 32768;
constexpr uint32_t SCE_SAVE_DATA_BLOCKS_MIN2              = 96;
constexpr uint32_t SCE_SAVE_DATA_BLOCKS_MAX               = 32768;
constexpr uint32_t SCE_SAVE_DATA_MEMORY_MAXSIZE3          = 32 * 1024 * 1024;
constexpr uint32_t SCE_SAVE_DATA_MEMORY_SETUP_MAX_COUNT   = 4;
constexpr uint32_t SCE_SAVE_DATA_MEMORY_DATANUM_MAX_COUNT = 5;

constexpr uint32_t SCE_SAVE_DATA_NUM_MOUNTS = 15;

constexpr std::string_view SCE_SAVE_DATA_DIRNAME_SAVE_DATA_MEMORY        = "sce_sdmemory";
constexpr std::string_view SCE_SAVE_DATA_DIRNAME_SAVE_DATA_MEMORY_SLOT_0 = "sce_sdmemory";
constexpr std::string_view SCE_SAVE_DATA_DIRNAME_SAVE_DATA_MEMORY_SLOT_1 = "sce_sdmemory1";
constexpr std::string_view SCE_SAVE_DATA_DIRNAME_SAVE_DATA_MEMORY_SLOT_2 = "sce_sdmemory2";
constexpr std::string_view SCE_SAVE_DATA_DIRNAME_SAVE_DATA_MEMORY_SLOT_3 = "sce_sdmemory3";

constexpr std::string_view SCE_SAVE_DATA_FILENAME_SAVE_DATA_MEMORY = "memory.dat";