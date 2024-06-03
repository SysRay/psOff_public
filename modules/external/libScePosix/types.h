#pragma once
#include "codes.h"

struct SceNetId {};

struct SceNetSaFamily_t {};

struct SceNetSockaddr {
  uint8_t          sa_len;
  SceNetSaFamily_t sa_family;
  char             sa_data[14];
};

struct SceNetSocklen_t {};
