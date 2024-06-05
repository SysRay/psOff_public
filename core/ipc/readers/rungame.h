#pragma once

#include <cstdint>
#include <string>

class IPCRunGame {
  std::string m_mainExec;
  std::string m_mainRoot;
  std::string m_updateRoot;

  public:
  IPCRunGame(const char* data, uint32_t size) {
    m_mainExec   = data;
    m_mainRoot   = data += (m_mainExec.size() + 1);
    m_updateRoot = data += (m_mainRoot.size() + 1);
  }

  const std::string& getUpdate() { return m_updateRoot; }

  const std::string& getExecutable() { return m_mainExec; }

  const std::string& getRoot() { return m_mainRoot; }
};
