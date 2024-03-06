#pragma once
#include <stdint.h>

namespace Err {} // namespace Err

constexpr size_t PROGNAME_MAX_SIZE = 511;

constexpr uint8_t SCE_KERNEL_PROT_CPU_READ  = 0x01;
constexpr uint8_t SCE_KERNEL_PROT_CPU_RW    = 0x02;
constexpr uint8_t SCE_KERNEL_PROT_CPU_EXEC  = 0x04;
constexpr uint8_t SCE_KERNEL_PROT_CPU_ALL   = 0x07;
constexpr uint8_t SCE_KERNEL_PROT_GPU_READ  = 0x10;
constexpr uint8_t SCE_KERNEL_PROT_GPU_WRITE = 0x20;
constexpr uint8_t SCE_KERNEL_PROT_GPU_RW    = 0x30;
constexpr uint8_t SCE_KERNEL_PROT_GPU_ALL   = 0x30;

constexpr uint32_t SCE_KERNEL_PAGE_SIZE = 16384;

constexpr uint32_t SCE_KERNEL_AIO_SCHED_WINDOW_MAX      = 128;
constexpr uint32_t SCE_KERNEL_AIO_DELAYED_COUNT_MAX     = 128;
constexpr uint32_t SCE_KERNEL_AIO_ENABLE_SPLIT          = 1;
constexpr uint32_t SCE_KERNEL_AIO_DISABLE_SPLIT         = 0;
constexpr uint32_t SCE_KERNEL_AIO_SPLIT_SIZE_MAX        = 0x1000000;
constexpr uint32_t SCE_KERNEL_AIO_SPLIT_CHUNK_SIZE_MAX  = 0x1000000;
constexpr uint32_t SCE_KERNEL_AIO_SCHED_WINDOW_DEFAULT  = 32;
constexpr uint32_t SCE_KERNEL_AIO_DELAYED_COUNT_DEFAULT = 32;
constexpr uint32_t SCE_KERNEL_AIO_SPLIT_SIZE_DEFAULT    = 0x100000;
constexpr uint32_t SCE_KERNEL_AIO_REQUEST_NUM_MAX       = 128;
constexpr uint32_t SCE_KERNEL_AIO_ID_NUM_MAX            = 128;