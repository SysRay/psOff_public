#pragma once
#include "utility/utility.h"

extern "C" {
SYSV_ABI void jmpEntry(uint64_t addr, void const* entryParams, void* exitHandler);
}
