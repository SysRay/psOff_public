#pragma once
#include <stdint.h>

namespace Err {} // namespace Err

constexpr uint16_t SCE_KERNEL_MS_SYNC       = 0;
constexpr uint16_t SCE_KERNEL_MS_ASYNC      = 1;
constexpr uint16_t SCE_KERNEL_MS_INVALIDATE = 2;

constexpr uint16_t SCE_KERNEL_PRIO_FIFO_DEFAULT = 700;
constexpr uint16_t SCE_KERNEL_PRIO_FIFO_HIGHEST = 256;
constexpr uint16_t SCE_KERNEL_PRIO_FIFO_LOWEST  = 767;