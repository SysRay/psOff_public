#include "../ipcpacket.h"

class IPCEmulatorExecRequest: public IPCPacketRead {
  std::vector<const char*> m_args = {};

  uintptr_t m_execOffset;

  public:
  IPCEmulatorExecRequest(PipeProcess* pproc, IPCHeader* phead): IPCPacketRead(IpcEvent::EMU_EXEC_REQUEST, pproc, phead) {
    m_execOffset = 0;
    // todo
  }

  auto getExecutable() { return std::string_view(&m_data[m_execOffset]); }

  auto getArguments() { return &m_args; }
};
