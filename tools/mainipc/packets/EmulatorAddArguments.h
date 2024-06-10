#include "../ipcpacket.h"

class IPCEmulatorAddArguments: public IPCPacket {
  public:
  IPCEmulatorAddArguments(std::vector<const char*>* args): IPCPacket(IpcEvent::EMU_ADD_ARGS) {
    uint32_t         argc;
    std::string_view argcw((char*)&argc, 4);

    if (args == nullptr || args->size() == 0) {
      argc = 0;
      m_data.insert(m_data.end(), argcw.begin(), argcw.end());
      return;
    }

    argc = args->size();
    m_data.insert(m_data.end(), argcw.begin(), argcw.end());
    for (auto it = args->begin(); it != args->end(); ++it) {
      std::string_view arg(*it);
      m_data.insert(m_data.end(), arg.begin(), arg.end());
      m_data.push_back('\0');
    }
  }
};
