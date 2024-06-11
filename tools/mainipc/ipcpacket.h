#pragma once

#include "ipcevents.h"
#include "pipeprocess.h"
#include "types.h"

#include <cstdint>
#include <vector>

class IPCPacket {
  public:
  IPCPacket(IpcEvent id) {
    m_data.resize(sizeof(IPCHeader));
    auto hdr      = (IPCHeader*)m_data.data();
    hdr->packetId = (uint32_t)id;
    hdr->bodySize = 0;
  };

  void putPacketTo(PipeProcess* pproc) {
    std::unique_lock lock(pproc->wrMutex);

    auto hdr      = (IPCHeader*)m_data.data();
    hdr->bodySize = m_data.size() - sizeof(IPCHeader);

    pproc->vWriteData.insert(pproc->vWriteData.end(), m_data.begin(), m_data.end());
  }

  protected:
  IPCData m_data = {};
};

class IPCPacketRead {
  public:
  IPCPacketRead(IpcEvent id, PipeProcess* pproc, IPCHeader* phead) {
    if ((uint32_t)id != phead->packetId) throw std::exception("Invalid packet id");
    std::unique_lock lock(pproc->wrMutex);

    m_data.resize(phead->bodySize + sizeof(IPCHeader));
    *(IPCHeader*)m_data.data() = *phead;

    DWORD rd;
    if (pproc->read(m_data.data() + sizeof(IPCHeader), phead->bodySize) != phead->bodySize) throw std::exception("Failed to read packet");
  }

  protected:
  IPCData m_data;
};
