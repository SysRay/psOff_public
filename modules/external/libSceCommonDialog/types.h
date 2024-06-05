#pragma once
#include <stdint.h>

enum class SceCommonDialogStatus { NONE = 0, INITIALIZED = 1, RUNNING = 2, FINISHED = 3 };

enum class SceCommonDialogResult {
  OK            = 0,
  USER_CANCELED = 1,
};

struct alignas(8) SceCommonDialogBaseParam {
  size_t   size;
  uint8_t  reserved[36];
  uint32_t magic;
};

constexpr uint32_t SCE_COMMON_DIALOG_MAGIC_NUMBER = 0xC0D1A109;