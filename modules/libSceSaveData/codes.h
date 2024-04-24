#pragma once
#include <stdint.h>
#include <string_view>

namespace Err {
namespace SaveData {
constexpr int PARAMETER                            = -2137063424;
constexpr int NOT_INITIALIZED                      = -2137063423;
constexpr int OUT_OF_MEMORY                        = -2137063422;
constexpr int BUSY                                 = -2137063421;
constexpr int NOT_MOUNTED                          = -2137063420;
constexpr int NO_PERMISSION                        = -2137063419;
constexpr int FINGERPRINT_MISMATCH                 = -2137063418;
constexpr int EXISTS                               = -2137063417;
constexpr int NOT_FOUND                            = -2137063416;
constexpr int NO_SPACE_FS                          = -2137063414;
constexpr int INTERNAL                             = -2137063413;
constexpr int MOUNT_FULL                           = -2137063412;
constexpr int BAD_MOUNTED                          = -2137063411;
constexpr int FILE_NOT_FOUND                       = -2137063410;
constexpr int BROKEN                               = -2137063409;
constexpr int INVALID_LOGIN_USER                   = -2137063407;
constexpr int MEMORY_NOT_READY                     = -2137063406;
constexpr int BACKUP_BUSY                          = -2137063405;
constexpr int NOT_REGIST_CALLBACK                  = -2137063403;
constexpr int BUSY_FOR_SAVING                      = -2137063402;
constexpr int LIMITATION_OVER                      = -2137063401;
constexpr int EVENT_BUSY                           = -2137063400;
constexpr int PARAMSFO_TRANSFER_TITLE_ID_NOT_FOUND = -2137063399;
} // namespace SaveData
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
