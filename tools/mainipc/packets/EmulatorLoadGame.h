#include "../ipcpacket.h"

class IPCEmulatorLoadGame: public IPCPacket {
  std::vector<char> m_data = {};

  public:
  IPCEmulatorLoadGame(std::string_view appexec, std::string_view approot, std::string_view updateroot): m_data {}, IPCPacket(IpcEvent::EMU_LOAD_GAME, &m_data) {
    m_data.insert(m_data.end(), appexec.begin(), appexec.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), approot.begin(), approot.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), updateroot.begin(), updateroot.end());
    m_data.push_back('\0');
  }

  void addArguments(std::vector<const char*>* args) {
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
