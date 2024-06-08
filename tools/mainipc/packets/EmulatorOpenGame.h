#include "../packet.h"

class EmulatorOpenGamePacket: public PacketWriter {
  std::vector<char> m_data = {};

  public:
  EmulatorOpenGamePacket(std::string_view appexec, std::string_view approot, std::string_view updateroot): m_data {}, PacketWriter(0x01, &m_data) {
    m_data.insert(m_data.end(), appexec.begin(), appexec.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), approot.begin(), approot.end());
    m_data.push_back('\0');
    m_data.insert(m_data.end(), updateroot.begin(), updateroot.end());
    m_data.push_back('\0');
  }
};
