#pragma once
#include <memory>
#include <stdint.h>

namespace ExceptionHandler {

std::unique_ptr<uint8_t[]> install(uint64_t imageAddr, uint64_t imageSize);
} // namespace ExceptionHandler