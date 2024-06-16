#include "chunkreader.hpp"

#include "core/kernel/filesystem.h"
#include "logging.h"

#include <fstream>

LOG_DEFINE_MODULE(PlayGoChunkReader);

GoReader::GoReader(std::filesystem::path file) {
  LOG_USE_MODULE(PlayGoChunkReader);

  if ((m_fd = filesystem::open(file.string().c_str(), {.mode = filesystem::SceOpenMode::RDONLY}, 0)) < 0) {
    LOG_ERR(L"Failed to open playgo file!");
    return;
  }

  auto size = filesystem::lseek(m_fd, 0, SEEK_END);
  if (size < sizeof(playgo_header)) {
    LOG_ERR(L"Invalid playgo file!");
    return;
  }

  if (filesystem::mmap(nullptr, size, 1, {.mode = filesystem::SceMapMode::SHARED, .type = filesystem::SceMapType::FILE}, m_fd, 0, (void**)&m_data) != Ok) {
    LOG_ERR(L"Failed to map playgo file to memory");
    return;
  }

  if (m_header->magic != 0x6F676C70) {
    LOG_ERR(L"Invalid playgo file magic!");
    return;
  }

  auto getchunkptr = [this, size](const playgo_chunk* chunk) -> const void* {
    if (size < chunk->offset + chunk->size) return nullptr;
    return m_data + chunk->offset;
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
}

GoReader::~GoReader() {
  filesystem::munmap((void*)m_data, 0);
  filesystem::close(m_fd);
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
