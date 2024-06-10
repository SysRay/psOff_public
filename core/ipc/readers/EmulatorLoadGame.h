#pragma once

#include <cstdint>
#include <string>
#include <vector>

class IPCEmulatorLoadGameRead {
  std::string_view m_mainExec;
  std::string_view m_mainRoot;
  std::string_view m_updateRoot;

  public:
  IPCEmulatorLoadGameRead(const char* data, uint32_t size) {
    m_mainExec   = data;
    m_mainRoot   = data += (m_mainExec.size() + 1);
    m_updateRoot = data += (m_mainRoot.size() + 1);
  }

  const auto getUpdate() { return m_updateRoot; }

  const auto getExecutable() { return m_mainExec; }

  const auto getRoot() { return m_mainRoot; }
};
