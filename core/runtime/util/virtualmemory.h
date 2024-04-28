#pragma once

#include <stdint.h>

bool flushInstructionCache(uint64_t address, uint64_t size);
bool patchReplace(uint64_t vaddr, uint64_t const value);
