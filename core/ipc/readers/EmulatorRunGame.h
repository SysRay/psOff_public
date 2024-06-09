#pragma once

#include <cstdint>
#include <string>
#include <vector>

class IPCEmulatorRunGameRead {
  std::string_view         m_mainExec;
  std::string_view         m_mainRoot;
  std::string_view         m_updateRoot;
  std::vector<const char*> m_args;

  public:
  IPCEmulatorRunGameRead(const char* data, uint32_t size) {
    m_mainExec   = data;
    m_mainRoot   = data += (m_mainExec.size() + 1);
    m_updateRoot = data += (m_mainRoot.size() + 1);

    uint32_t argc = *(uint32_t*)(data + (m_updateRoot.size() + 1));
    if (argc > 4095) throw std::exception("Invalid arguments count");
    data += 4; // Skip these 4 bytes of uint32_t
    for (uint32_t i = 0; i < argc; ++i) {
      m_args.push_back(data);
      data += std::string_view(data).size() + 1;
    }
  }

  const auto getUpdate() { return m_updateRoot; }

  const auto getExecutable() { return m_mainExec; }

  const auto getRoot() { return m_mainRoot; }

  const auto& getArgs() { return m_args; }
};
