#pragma once

#include "../ipcpacket.h"

#include <cstdint>
#include <string>
#include <vector>

class IPCEmulatorLoadExec: public IPCPacket {
  public:
  IPCEmulatorLoadExec(std::string_view exec, char* const* args): IPCPacket(IpcEvent::EMU_EXEC_REQUEST) {
    m_data.insert(m_data.end(), exec.begin(), exec.end());
    m_data.push_back('\0');

    auto argc_it = m_data.end();
    m_data.insert(m_data.end(), {0, 0, 0, 0});
    if (args == nullptr) return;

    for (uint32_t i = 0; i < 4096; ++i) {
      if (args[i] == nullptr) {
        // Set the arguments count in packet
        *(uint32_t*)argc_it._Ptr = (i - 1);
        break;
      }

      auto str = std::string_view(args[i]);
      m_data.insert(m_data.end(), str.begin(), str.end());
      m_data.push_back('\0');
    }
  }
};
