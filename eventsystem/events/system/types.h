#pragma once
#include <cinttypes>

namespace events::system {
struct LoadExecData {
  const char*  exec;
  char* const* args;
};
} // namespace events::system
