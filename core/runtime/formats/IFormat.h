#pragma once
#include "../program.h"
#include "ISymbols.h"
#include "utility/utility.h"

#include <filesystem>
#include <memory>
#include <string>
#include <utility>

class IRuntimeLinker;

struct LibraryId {
  bool operator==(const LibraryId& other) const { return version == other.version && name == other.name; }

  std::string      id;
  int              version;
  std::string_view name;
};

class IFormat: public Symbols::IResolve {
  CLASS_NO_COPY(IFormat);

  protected:
  uint64_t m_baseVaddr; // Set with load2Mem
  size_t   m_fileSize;

  std::filesystem::path const m_path;     /// Fullpath
  std::filesystem::path const m_filename; /// Just the filename

  util::InterfaceState m_intState = util::InterfaceState::NotInit;

  IFormat(std::filesystem::path&& path): m_path(std::move(path)), m_filename(m_path.filename()) {}

  public:
  virtual ~IFormat() = default;

  virtual bool init() = 0;

  virtual bool load2Mem(Program* prog) = 0;

  virtual bool loadSegment(uint8_t* loadAddr, uint64_t fileOffset, uint64_t size) = 0;

  virtual void getAllocInfo(uint64_t& baseSize, uint64_t& baseSizeAligned, uint64_t& sizeAlloc) = 0;

  virtual bool containsAddr(uint64_t vaddr, uint64_t baseVaddr) = 0;

  virtual void setupMissingRelocationHandler(Program* prog, void* relocateHandler, void* payload) = 0;

  virtual Symbols::SymbolInfo getSymbolInfo(uint64_t const relIndex) const = 0;

  virtual void relocate(Program const* prog, uint64_t invalidMemoryAddr) = 0;

  virtual std::unordered_map<uint64_t, std::string_view> getDebugStrings() const                                                               = 0;
  virtual std::string                                    collectDebugInfos(std::unordered_map<uint64_t, std::string_view>& debugStrings) const = 0;

  virtual void dtInit(uint64_t base, size_t argc = 0, const void* argp = nullptr) const = 0;

  virtual void dtDeinit(uint64_t base) const = 0;

  virtual std::vector<std::pair<uint64_t, uint64_t>> getExecSections() = 0;

  auto getSizeFile() const { return m_fileSize; }

  auto getFilename() const { return m_filename; }

  auto getFilepath() const { return m_path; }
};