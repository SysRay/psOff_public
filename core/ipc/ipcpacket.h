#pragma once

#include "tools/mainipc/ipcevents.h"
#include "tools/mainipc/types.h"

class IPCPacket {
  public:
  IPCPacket(IpcEvent id) {
    m_data.resize(sizeof(IPCHeader));

    auto hdr      = (IPCHeader*)m_data.data();
    hdr->packetId = (uint32_t)id;
    hdr->bodySize = 0;
  }

  IPCHeader* getData() {
    auto hdr      = (IPCHeader*)m_data.data();
    hdr->bodySize = m_data.size() - sizeof(IPCHeader);

    return (IPCHeader*)m_data.data();
  }

  protected:
  IPCData m_data;
};
