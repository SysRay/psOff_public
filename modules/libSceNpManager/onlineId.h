#pragma once
#include <stdint.h>

constexpr uint32_t SCE_NP_ONLINEID_MIN_LENGTH = 3;
constexpr uint32_t SCE_NP_ONLINEID_MAX_LENGTH = 16;

/* Online ID */
struct SceNpOnlineId {
  char data[SCE_NP_ONLINEID_MAX_LENGTH];
  char term;
  char dummy[3];
};

/* NP ID */
struct SceNpId {
  SceNpOnlineId handle;
  uint8_t       opt[8];
  uint8_t       reserved[8];
};
