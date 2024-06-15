#include "chunkreader.hpp"

#include "logging.h"

#include <fstream>

LOG_DEFINE_MODULE(PlayGoChunkReader);

GoReader::GoReader(std::filesystem::path file) {
  LOG_USE_MODULE(PlayGoChunkReader);

  std::ifstream fstrm(file, std::ios::in | std::ios::binary);

  if (fstrm.is_open()) {
    if (!fstrm.read((char*)&m_header, sizeof(m_header))) {
      LOG_ERR(L"Failed to read playgo file header");
      return;
    }

    fstrm.seekg(0, std::ios::end);
    m_buffer.resize(fstrm.tellg());
    fstrm.seekg(0);
    fstrm.read(m_buffer.data(), m_buffer.size());

    if (m_buffer.size() < sizeof(playgo_header)) {
      LOG_ERR(L"Invalid playgo file!");
      return;
    }

    m_header = (playgo_header*)m_buffer.data();

    if (m_header->magic != 0x6F676C70) {
      LOG_ERR(L"Invalid playgo file magic!");
      return;
    }

    auto getchunkptr = [this](const playgo_chunk* chunk) -> void* {
      if (m_buffer.size() < chunk->offset + chunk->size) return nullptr;
      return m_buffer.data() + chunk->offset;
    };

    if ((m_chunkattr_ent = (playgo_chunkattr_ent*)getchunkptr(&(m_header->chunk_attrs))) == nullptr) {
      LOG_ERR(L"Failed to read chunkattr section");
      return;
    }

    if ((m_mchunks = (uint16_t*)getchunkptr(&(m_header->chunk_mchunks))) == nullptr) {
      LOG_ERR(L"Failed to read mchunks section");
      return;
    }

    if ((m_labels = (const char*)getchunkptr(&(m_header->chunk_labels))) == nullptr) {
      LOG_ERR(L"Failed to read labels section");
      return;
    }

    if ((m_mchunkattr_ent = (playgo_mchunk_attr_ent*)getchunkptr(&(m_header->mchunk_attrs))) == nullptr) {
      LOG_ERR(L"Failed to read mchunk_attr section");
      return;
    }

    m_loaded = true;
    return;
  }

  LOG_ERR(L"Failed to open %s", file.c_str());
}

uint64_t GoReader::getChunkSize(uint16_t id) {
  if (!m_loaded || id >= getChunksCount()) return 0ull;

  uint64_t total_size = 0;

  if (auto mchunks_len = m_chunkattr_ent[id].mchunk_count) {
    auto mchunks = (uint16_t*)((char*)m_mchunks + m_chunkattr_ent[id].mchunks_offset);

    for (int i = 0; i < mchunks_len; ++i) {
      auto mchunk_id = mchunks[i];
      total_size += m_mchunkattr_ent[mchunk_id].size.value;
    }
  }

  return total_size;
}
