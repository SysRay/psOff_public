#pragma once

#include <filesystem>

class GoReader {
  public:
  GoReader(std::filesystem::path file);
  virtual ~GoReader() = default;

  uint32_t getChunksCount() const { return m_loaded ? m_header->chunk_count : 0; };

  uint64_t getChunkSize(uint16_t id);

  private:
  bool m_loaded = false;

#pragma pack(push, 1)

  struct playgo_chunk {
    uint32_t offset;
    uint32_t size;
  };

  struct playgo_header {
    uint32_t magic;

    struct {
      uint16_t major;
      uint16_t minor;
    } version;

    uint16_t image_count;
    uint16_t chunk_count;
    uint16_t mchunk_count;
    uint16_t scenario_count;

    uint32_t file_size;
    uint16_t def_scenario_id;
    uint16_t attrib;
    uint32_t sdk_version;
    uint16_t disc_count;
    uint16_t layer_bmp;

    char reserved[32];

    char content_id[128];

    playgo_chunk chunk_attrs;

    playgo_chunk chunk_mchunks;

    playgo_chunk chunk_labels;

    playgo_chunk mchunk_attrs;

    playgo_chunk scenario_attrs;

    playgo_chunk scenario_chunks;

    playgo_chunk scenario_labels;

    playgo_chunk inner_mchunk_attrs;
  };

  struct playgo_chunkattr_ent {
    uint8_t  flags;
    uint8_t  image_disc_layer_n;
    uint8_t  req_locus;
    uint8_t  unknown[11];
    uint16_t mchunk_count;
    uint64_t lang_mask;
    uint32_t mchunks_offset;
    uint32_t label_offset;
  };

  struct playgo_mchunk_attr_ent {
    uint64_t loc;

    struct {
      uint64_t align : 16;
      uint64_t value : 48;
    } size;
  };

#pragma pack(pop)

  std::vector<char> m_buffer;

  const playgo_header*          m_header         = nullptr;
  const playgo_chunkattr_ent*   m_chunkattr_ent  = nullptr;
  const playgo_mchunk_attr_ent* m_mchunkattr_ent = nullptr;
  const uint16_t*               m_mchunks        = nullptr;
  const char*                   m_labels         = nullptr;
};
