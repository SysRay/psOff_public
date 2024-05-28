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

    if (m_header.magic != 0x6F676C70) {
      LOG_ERR(L"Invalid playgo file magic!");
      return;
    }

    auto chunk_loader = [&fstrm](playgo_chunk* chunk, void** ptr) -> bool {
      auto buf = new char[chunk->size];

      if (fstrm.seekg(chunk->offset, std::ios::beg) && fstrm.read(buf, chunk->size)) {
        *ptr = buf;
        return true;
      }

      delete[] buf;
      return false;
    };

    if (!chunk_loader(&m_header.chunk_attrs, (void**)&m_chunkattr_ent)) {
      LOG_ERR(L"Failed to read playgo chunk attributes!");
      return;
    }

    if (!chunk_loader(&m_header.chunk_mchunks, (void**)&m_mchunks)) {
      LOG_ERR(L"Failed to read playgo chunk attributes!");
      return;
    }

    if (!chunk_loader(&m_header.chunk_labels, (void**)&m_labels)) {
      LOG_ERR(L"Failed to read playgo chunk attributes!");
      return;
    }

    if (!chunk_loader(&m_header.mchunk_attrs, (void**)&m_mchunattr_ent)) {
      LOG_ERR(L"Failed to read playgo chunk attributes!");
      return;
    }

    m_loaded = true;
    return;
  }

  LOG_ERR(L"Failed to open %s", file.c_str());
}

GoReader::~GoReader() {
  if (m_chunkattr_ent) delete[] m_chunkattr_ent;
  if (m_mchunattr_ent) delete[] m_mchunattr_ent;
  if (m_mchunks) delete[] m_mchunks;
  if (m_labels) delete[] m_labels;
}

uint64_t GoReader::getChunkSize(uint16_t id) {
  if (!m_loaded || id >= getChunksCount()) return 0ull;

  uint64_t total_size = 0;

  if (auto mchunks_len = m_chunkattr_ent[id].mchunk_count) {
    auto mchunks = &m_mchunks[m_chunkattr_ent[id].mchunks_offset / sizeof(*m_mchunks)];

    for (int i = 0; i < mchunks_len; ++i) {
      auto mchunk_id = mchunks[i];
      total_size += m_mchunattr_ent[mchunk_id].size;
    }
  }

  return total_size;
}
