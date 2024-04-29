#pragma once

#include "../imemory.h"

class IMemoryManager;
std::unique_ptr<IMemoryType> createDirectMemory(IMemoryManager* parent);