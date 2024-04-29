#pragma once

#include <cstdint>

constexpr uint64_t SYSTEM_RESERVED = 0x800000000u;
constexpr uint64_t INVALID_OFFSET  = 0x040000000u;

constexpr uint64_t INVALID_MEMORY = SYSTEM_RESERVED + INVALID_OFFSET;

constexpr size_t XSAVE_BUFFER_SIZE = 2688;
