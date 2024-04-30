#pragma once
#include <memory>
#include <stdint.h>

namespace ExceptionHandler {

uint64_t getAllocSize();
void     install(uint64_t imageAddr, uint64_t handlerDstAddr, uint64_t imageSize);

} // namespace ExceptionHandler