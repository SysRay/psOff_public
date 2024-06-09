#pragma once

#include "core/ipc/events.h"
#include "pipeprocess.h"

#include <cstdint>
#include <vector>

struct IPCHeader {
  uint32_t packetId;
  uint32_t packetSize;
};

class IPCPacket {
  public:
  IPCPacket(IpcEvent id, std::vector<char>* data): m_header({(uint32_t)id, 0}), m_data(data) {};

  void putPacketTo(PipeProcess* pproc) {
    std::unique_lock lock(pproc->wrMutex);
    m_header.packetSize = m_data->size();
    std::string_view sv((char*)&m_header, sizeof(m_header));
    pproc->vWriteData.insert(pproc->vWriteData.end(), sv.begin(), sv.end());
    pproc->vWriteData.insert(pproc->vWriteData.end(), m_data->begin(), m_data->end());
  }

  bool readPacketFrom(PipeProcess* pproc, IPCHeader* phead) {
    if (m_header.packetId != phead->packetId) return false;
    m_header.packetSize = phead->packetSize;
    m_data->resize(m_header.packetSize);

    DWORD rd;
    return ReadFile(pproc->hPipe, m_data->data(), m_header.packetSize, &rd, nullptr) && rd > 0;
  }

  private:
  IPCHeader m_header;

  std::vector<char>* m_data;
};
