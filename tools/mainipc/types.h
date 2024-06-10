#pragma once

#include <cstdint>
#include <vector>

struct IPCHeader {
  uint32_t packetId;
  uint32_t bodySize;
};

typedef std::vector<char> IPCData;
