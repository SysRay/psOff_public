#include "../ipcpacket.h"

class IPCEmulatorExecRequest: public IPCPacketRead {
  std::vector<char>        m_data = {};
  std::vector<const char*> m_args = {};

  uintptr_t m_execOffset;

  public:
  IPCEmulatorExecRequest(PipeProcess* pproc, IPCHeader* phead): m_data {}, IPCPacketRead(IpcEvent::EMU_EXEC_REQUEST, &m_data, pproc, phead) {
    m_execOffset = 0;
    // todo
  }

  auto getExecutable() { return std::string_view(&m_data[m_execOffset]); }

  auto getArguments() { return &m_args; }
};
