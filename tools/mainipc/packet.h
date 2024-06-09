#pragma once

#include "pipeprocess.h"

#include <cstdint>
#include <vector>

struct PacketHeader {
  uint32_t packetId;
  uint32_t packetSize;
};

class PacketWriter {
  PacketHeader m_header;

  std::vector<char>* m_data;

  public:
  PacketWriter(uint32_t id, std::vector<char>* data): m_header({id, 0}), m_data(data) {};

  void putPacketTo(PipeProcess* pproc) {
    std::unique_lock lock(pproc->wrMutex);
    m_header.packetSize = m_data->size();
    std::string_view sv((char*)&m_header, sizeof(m_header));
    pproc->vWriteData.insert(pproc->vWriteData.end(), sv.begin(), sv.end());
    pproc->vWriteData.insert(pproc->vWriteData.end(), m_data->begin(), m_data->end());
  }

  bool readPacketFrom(PipeProcess* pproc, PacketHeader* phead) {
    if (m_header.packetId != phead->packetId) return false;
    m_header.packetSize = phead->packetSize;
    m_data->resize(m_header.packetSize);

    DWORD rd;
    return ReadFile(pproc->hPipe, m_data->data(), m_header.packetSize, &rd, nullptr) && rd > 0;
  }
};
