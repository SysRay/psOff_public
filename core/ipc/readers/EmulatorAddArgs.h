#pragma once

#include <cstdint>
#include <string>
#include <vector>

class IPCEmulatorAddArgsRead {
  std::vector<const char*> m_args;

  public:
  IPCEmulatorAddArgsRead(const char* data, uint32_t size) {
    uint32_t argc = *(uint32_t*)data;
    if (argc > 4095) throw std::exception("Invalid arguments count");
    data += 4; // Skip these 4 bytes of uint32_t
    for (uint32_t i = 0; i < argc; ++i) {
      m_args.push_back(data);
      data += std::string_view(data).size() + 1;
    }
  }

  const auto& getArgs() { return m_args; }
};
