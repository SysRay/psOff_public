#include "../ipcpacket.h"

class IPCEmulatorRunGame: public IPCPacket {
  std::vector<char> m_data = {};

  public:
  IPCEmulatorRunGame(): m_data {}, IPCPacket(IpcEvent::EMU_RUN_GAME, &m_data) {}
};
