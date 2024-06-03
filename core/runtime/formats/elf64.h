#pragma once

#include "IFormat.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>

namespace elf64 {

#pragma pack(push, 1) // Structs match file data

// ### Custom Structs
struct CustomHeader {
  uint8_t  ident[12];
  uint16_t size1;
  uint16_t size2;
  uint64_t sizeFile;
  uint16_t numSegments;
  uint16_t unknown;
  uint32_t pad;
};

struct SegmentHeader {
  uint64_t type;
  uint64_t offset;
  uint64_t sizeCom;   /// compressed
  uint64_t sizeDecom; /// decompressed
};

constexpr unsigned char EI_ABIVERSION = 8;

constexpr uint64_t ET_EXEC        = 0xfe00; /// Exe
constexpr uint64_t ET_DYNEXEC     = 0xfe10; /// Exe
constexpr uint64_t ET_DYNAMIC     = 0xfe18; /// Lib
constexpr uint64_t EM_ARCH_X86_64 = 0x3e;

// ### Program Segment type
constexpr uint64_t PH_LOAD           = 1;
constexpr uint64_t PH_DYNAMIC        = 2;
constexpr uint64_t PH_TLS            = 7;
constexpr uint64_t PH_OS_DYNLIBDATA  = 0x61000000;
constexpr uint64_t PH_OS_PROCPARAM   = 0x61000001;
constexpr uint64_t PH_OS_MODULEPARAM = 0x61000002;
constexpr uint64_t PH_OS_RELRO       = 0x61000010;
constexpr uint64_t PH_GNU_EH_HEADER  = 0x6474e550;

constexpr uint64_t SF_ORDR = 0x1;   // ordered?
constexpr uint64_t SF_ENCR = 0x2;   // encrypted
constexpr uint64_t SF_SIGN = 0x4;   // signed
constexpr uint64_t SF_DFLG = 0x8;   // deflated
constexpr uint64_t SF_BFLG = 0x800; // block segment

constexpr int64_t DT_DEBUG                  = 0x00000015;
constexpr int64_t DT_FINI                   = 0x0000000d;
constexpr int64_t DT_FINI_ARRAY             = 0x0000001a;
constexpr int64_t DT_FINI_ARRAYSZ           = 0x0000001c;
constexpr int64_t DT_FLAGS                  = 0x0000001e;
constexpr int64_t DT_INIT                   = 0x0000000c;
constexpr int64_t DT_INIT_ARRAY             = 0x00000019;
constexpr int64_t DT_INIT_ARRAYSZ           = 0x0000001b;
constexpr int64_t DT_NEEDED                 = 0x00000001;
constexpr int64_t DT_OS_EXPORT_LIB          = 0x61000013;
constexpr int64_t DT_OS_EXPORT_LIB_1        = 0x61000047;
constexpr int64_t DT_OS_EXPORT_LIB_ATTR     = 0x61000017;
constexpr int64_t DT_OS_FINGERPRINT         = 0x61000007;
constexpr int64_t DT_OS_HASH                = 0x61000025;
constexpr int64_t DT_OS_HASHSZ              = 0x6100003d;
constexpr int64_t DT_OS_IMPORT_LIB          = 0x61000015;
constexpr int64_t DT_OS_IMPORT_LIB_1        = 0x61000049;
constexpr int64_t DT_OS_IMPORT_LIB_ATTR     = 0x61000019;
constexpr int64_t DT_OS_JMPREL              = 0x61000029;
constexpr int64_t DT_OS_MODULE_ATTR         = 0x61000011;
constexpr int64_t DT_OS_MODULE_INFO         = 0x6100000d;
constexpr int64_t DT_OS_MODULE_INFO_1       = 0x61000043;
constexpr int64_t DT_OS_NEEDED_MODULE       = 0x6100000f;
constexpr int64_t DT_OS_NEEDED_MODULE_1     = 0x61000045;
constexpr int64_t DT_OS_ORIGINAL_FILENAME   = 0x61000009;
constexpr int64_t DT_OS_ORIGINAL_FILENAME_1 = 0x61000041;
constexpr int64_t DT_OS_PLTGOT              = 0x61000027;
constexpr int64_t DT_OS_PLTREL              = 0x6100002b;
constexpr int64_t DT_OS_PLTRELSZ            = 0x6100002d;
constexpr int64_t DT_OS_RELA                = 0x6100002f;
constexpr int64_t DT_OS_RELAENT             = 0x61000033;
constexpr int64_t DT_OS_RELASZ              = 0x61000031;
constexpr int64_t DT_OS_STRSZ               = 0x61000037;
constexpr int64_t DT_OS_STRTAB              = 0x61000035;
constexpr int64_t DT_OS_SYMENT              = 0x6100003b;
constexpr int64_t DT_OS_SYMTAB              = 0x61000039;
constexpr int64_t DT_OS_SYMTABSZ            = 0x6100003f;
constexpr int64_t DT_PREINIT_ARRAY          = 0x00000020;
constexpr int64_t DT_PREINIT_ARRAYSZ        = 0x00000021;
constexpr int64_t DT_REL                    = 0x00000011;
constexpr int64_t DT_RELA                   = 0x00000007;
constexpr int64_t DT_SONAME                 = 0x0000000e;
constexpr int64_t DT_TEXTREL                = 0x00000016;

constexpr int64_t DT_HASH      = 0x00000004;
constexpr int64_t DT_STRTAB    = 0x00000005;
constexpr int64_t DT_STRSZ     = 0x0000000a;
constexpr int64_t DT_SYMTAB    = 0x00000006;
constexpr int64_t DT_SYMENT    = 0x0000000b;
constexpr int64_t DT_PLTGOT    = 0x00000003;
constexpr int64_t DT_PLTREL    = 0x00000014;
constexpr int64_t DT_JMPREL    = 0x00000017;
constexpr int64_t DT_PLTRELSZ  = 0x00000002;
constexpr int64_t DT_RELASZ    = 0x00000008;
constexpr int64_t DT_RELAENT   = 0x00000009;
constexpr int64_t DT_RELACOUNT = 0x6ffffff9;

constexpr uint8_t STB_LOCAL  = 0;
constexpr uint8_t STB_GLOBAL = 1;
constexpr uint8_t STB_WEAK   = 2;

constexpr uint8_t STT_NOTYPE = 0;
constexpr uint8_t STT_OBJECT = 1;
constexpr uint8_t STT_FUNC   = 2;

constexpr uint32_t R_X86_64_64        = 1;
constexpr uint32_t R_X86_64_GLOB_DAT  = 6;
constexpr uint32_t R_X86_64_JUMP_SLOT = 7;
constexpr uint32_t R_X86_64_RELATIVE  = 8;
constexpr uint32_t R_X86_64_DTPMOD64  = 16;
constexpr uint32_t R_X86_64_TPOFF64   = 18;

// DWARF
constexpr uint8_t DW_EH_PE_omit    = 0xff;
constexpr uint8_t DW_EH_PE_uleb128 = 0x01;
constexpr uint8_t DW_EH_PE_udata2  = 0x02;
constexpr uint8_t DW_EH_PE_udata4  = 0x03;
constexpr uint8_t DW_EH_PE_udata8  = 0x04;
constexpr uint8_t DW_EH_PE_sleb128 = 0x09;
constexpr uint8_t DW_EH_PE_sdata2  = 0x0A;
constexpr uint8_t DW_EH_PE_sdata4  = 0x0B;
constexpr uint8_t DW_EH_PE_sdata8  = 0x0C;

constexpr uint8_t DW_EH_PE_absptr  = 0x00;
constexpr uint8_t DW_EH_PE_pcrel   = 0x10;
constexpr uint8_t DW_EH_PE_datarel = 0x30;

// ### ELF Structs

struct ElfHeader {
  unsigned char ident[16];
  uint16_t      type;
  uint16_t      machine;
  uint32_t      version;
  uint64_t      entry;
  uint64_t      phOff;
  uint64_t      shOff;
  uint32_t      flags;
  uint16_t      sizeEh; // Elf header size
  uint16_t      sizePh; /// Program segment Header size
  uint16_t      numPh;
  uint16_t      sizeSh; /// Section Header size
  uint16_t      numSh;
  uint16_t      indexSh;
};

struct ProgramHeader {
  uint32_t type;
  uint32_t flags;
  uint64_t offset;
  uint64_t vaddr;
  uint64_t paddr;
  uint64_t sizeFile;
  uint64_t sizeMem;
  uint64_t align;
};

struct SectionHeader {
  uint32_t name;
  uint32_t type;
  uint64_t flags;
  uint64_t addr;
  uint64_t offset;
  uint64_t size;
  uint32_t link;
  uint32_t info;
  uint64_t addrAlign;
  uint64_t entrySize;
};

struct DynamicHeader {
  uint64_t tag;

  union {
    uint64_t value;
    uint64_t ptr;
  } _un;
};

struct Elf64_Sym {
  unsigned char getBind() const { return info >> 4u; }

  unsigned char getType() const { return info & 0xfu; }

  uint32_t      name;
  unsigned char info;
  unsigned char other;
  uint16_t      shndx;
  uint64_t      value;
  uint64_t      size;
};

struct Elf64_Rela {
  auto getSymbol() const { return (uint32_t)(info >> 32u); }

  auto getType() const { return (uint32_t)(info & 0xffffffff); }

  uint64_t offset;
  uint64_t info;
  int64_t  addend;
};

#pragma pack(pop)

struct ModuleId {
  bool operator==(const ModuleId& other) const { return versionMajor == other.versionMajor && versionMinor == other.versionMinor && name == other.name; }

  std::string      id;
  int              versionMajor;
  int              versionMinor;
  std::string_view name;
};

class SymbolMap {
  uint32_t const* const m_hashTable;
  uint64_t const        m_hashTableSize;
  char const* const     m_strTable;
  uint64_t const        m_strTableSize;
  Elf64_Sym const*      m_symbolTable;
  uint64_t const        m_symbolTableSizeTotal;

  public:
  SymbolMap(uint32_t const* hashTable, uint64_t hashTableSize, char const* strTable, uint64_t strTableSize, Elf64_Sym const* symbolTable,
            uint64_t symbolTableSizeTotal)
      : m_hashTable(hashTable),
        m_hashTableSize(hashTableSize),
        m_strTable(strTable),
        m_strTableSize(strTableSize),
        m_symbolTable(symbolTable),
        m_symbolTableSizeTotal(symbolTableSizeTotal) {}

  size_t size() const { return m_symbolTableSizeTotal; }

  // e.g. symname = "LHuSmO3SLd8#libc#libc"
  Elf64_Sym const* find(std::string_view symname) const;

  // eg "Qoo175Ig+-k", "libc", "libc"
  elf64::Elf64_Sym const* find(std::string_view symname, std::string_view libName, std::string_view modName) const;

  Elf64_Sym const& getSymbol(size_t index) const { return m_symbolTable[index]; };

  std::string_view getName(Elf64_Sym const& symbol) const { return m_strTable + symbol.name; };
};

struct DynamicInfo {
  SymbolMap const symbolsMap;

  DynamicInfo(SymbolMap&& symbolsMap): symbolsMap(std::move(symbolsMap)) {}

  std::optional<uint64_t> vaddrInit;
  std::optional<uint64_t> vaddrFini         = 0;
  std::optional<uint64_t> vaddrInitArray    = 0;
  std::optional<uint64_t> vaddrFinArray     = 0;
  std::optional<uint64_t> vaddrPreinitArray = 0;
  uint64_t                vaddrPltgot       = 0;

  uint64_t sizeInitArray    = 0;
  uint64_t sizeFiniArray    = 0;
  uint64_t sizePreinitArray = 0;

  Elf64_Rela const* jmprelaTable     = nullptr;
  uint64_t          jmprelaTableSize = 0;

  Elf64_Rela const* relaTable          = nullptr;
  uint64_t          relaTableTotalSize = 0;
  uint64_t          relaTableEntrySize = 0;

  uint64_t relativeCount = 0;

  uint64_t debug   = 0;
  uint64_t textrel = 0;
  uint64_t flags   = 0;

  std::unordered_map<std::string_view, LibraryId> exportedLibs;
  std::unordered_map<std::string_view, ModuleId>  exportedMods;
  std::unordered_map<std::string_view, LibraryId> importedLibs;
  std::unordered_map<std::string_view, ModuleId>  importedMods;
};

void dump(IFormat* format, std::fstream& outFile, std::fstream& mapFile);
} // namespace elf64