#pragma once

#include <cstdint>
#include <string>

class IPCEmulatorRunGameRead {
  std::string_view m_mainExec;
  std::string_view m_mainRoot;
  std::string_view m_updateRoot;

  public:
  IPCEmulatorRunGameRead(const char* data, uint32_t size) {
    m_mainExec   = data;
    m_mainRoot   = data += (m_mainExec.size() + 1);
    m_updateRoot = data += (m_mainRoot.size() + 1);
  }

  const std::string_view getUpdate() { return m_updateRoot; }

  const std::string_view getExecutable() { return m_mainExec; }

  const std::string_view getRoot() { return m_mainRoot; }
};
