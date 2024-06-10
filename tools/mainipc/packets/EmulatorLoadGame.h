#include "../ipcpacket.h"

class IPCEmulatorLoadGame: public IPCPacket {
  public:
  IPCEmulatorLoadGame(std::string_view appexec, std::string_view approot, std::string_view updateroot): IPCPacket(IpcEvent::EMU_LOAD_GAME) {
    m_data.insert(m_data.end(), appexec.begin(), appexec.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), approot.begin(), approot.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), updateroot.begin(), updateroot.end());
    m_data.push_back('\0');
  }
};
