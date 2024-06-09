#include "../ipcpacket.h"

class IPCEmulatorExecRequest: public IPCPacket {
  std::vector<char> m_data = {};

  public:
  IPCEmulatorExecRequest(): m_data {}, IPCPacket(IpcEvent::EMU_EXEC_REQUEST, &m_data) {}
};
