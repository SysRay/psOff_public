#include "../ipcpacket.h"

class IPCEmulatorRunGame: public IPCPacket {
  std::vector<char> m_data = {};

  public:
  IPCEmulatorRunGame(std::string_view appexec, std::string_view approot, std::string_view updateroot): m_data {}, IPCPacket(IpcEvent::EMU_RUN_GAME, &m_data) {
    m_data.insert(m_data.end(), appexec.begin(), appexec.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), approot.begin(), approot.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), updateroot.begin(), updateroot.end());
    m_data.push_back('\0');
  }
};
