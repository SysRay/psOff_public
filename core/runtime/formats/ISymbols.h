#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct LibraryId;

namespace Symbols {
enum class SymbolType {
  Unknown,
  Func,
  Object,
  TlsModule,
  NoType,
};

enum class BindType { Unknown, Local, Global, Weak };

struct Info {
  std::string_view libraryName; /// Fullname
  int              libraryVersion;
  std::string_view modulName; /// Fullname
  int              moduleVersionMajor;
  int              moduleVersionMinor;

  bool operator==(Info const& lhs) const {
    return libraryName == lhs.libraryName && libraryVersion >= lhs.libraryVersion && modulName == lhs.modulName &&
           moduleVersionMajor >= lhs.moduleVersionMajor && moduleVersionMinor >= lhs.moduleVersionMinor;
  }
};

struct SymbolInfo: public Info {
  std::string_view name;
  SymbolType       type;
};

struct Record {
  std::string_view name  = {};
  uint64_t         vaddr = 0;
};

struct RelocationInfo {
  bool             resolved   = false;
  BindType         bind       = BindType::Unknown;
  SymbolType       type       = SymbolType::Unknown;
  uint64_t         value      = 0;
  uint64_t         vaddr      = 0;
  uint64_t         base_vaddr = 0;
  std::string_view name;
  bool             bind_self = false;
};

class IResolve {
  public:
  virtual uintptr_t getAddress(std::string_view symName, std::string_view libName, std::string_view modName) const = 0;
  virtual uintptr_t getAddress(std::string_view symName) const                                                     = 0;

  virtual std::unordered_map<std::string_view, LibraryId> const& getExportedLibs() const = 0;
  virtual std::unordered_map<std::string_view, LibraryId> const& getImportedLibs() const = 0;
};

struct SymbolExport {
  struct Symbol {
    std::string name;
    std::string dbgName;
    uint64_t    vaddr;
    SymbolType  type;
  };

  std::string libraryName;
  int         libraryVersion;
  std::string modulName;
  int         moduleVersionMajor;
  int         moduleVersionMinor;

  std::unordered_map<std::string, Symbol> symbolsMap;

  SymbolExport(std::string_view libraryName, int libraryVersion, std::string_view modulName, int moduleVersionMajor, int moduleVersionMinor)
      : libraryName(libraryName),
        libraryVersion(libraryVersion),
        modulName(modulName),
        moduleVersionMajor(moduleVersionMajor),
        moduleVersionMinor(moduleVersionMinor) {}
};

struct SymbolIntercept {
  uint64_t vaddr;

  std::string_view name;
  std::string_view libraryName;
  std::string_view modulName;
};
} // namespace Symbols