#include "elf64.h"
#define __APICALL_EXTERN
#include "../runtimeLinker.h"
#undef __APICALL_EXTERN

#include "../memoryLayout.h"
#include "../util/exceptionHandler.h"
#include "../util/plt.h"
#include "../util/virtualmemory.h"
#include "core/memory/memory.h"

#include <algorithm>
#include <assert.h>
#include <cassert>
#include <cstring>
#include <format>
#include <fstream>
#include <logging.h>
#include <magic_enum/magic_enum.hpp>
#include <unordered_set>

namespace {
LOG_DEFINE_MODULE(ELF64);

// clang-format off

using module_func_t          = SYSV_ABI int (*)(size_t args, const void* argp);
using module_call_func_t     = SYSV_ABI void (*)();
using module_ini_fini_func_t = SYSV_ABI int (*)(size_t args, const void* argp, module_func_t func);

// clang-format on

int jmpModule(uint64_t addr, size_t args, const void* argp, module_func_t func) {
  return reinterpret_cast<module_ini_fini_func_t>(addr)(args, argp, func);
}

void callModule(uint64_t addr) {
  reinterpret_cast<module_call_func_t>(addr)();
}

bool isValid(elf64::CustomHeader const& h) {
  if (!std::equal(std::begin(h.ident), std::end(h.ident), std::begin({0x4f, 0x15, 0x3d, 0x1d, 0x00, 0x01, 0x01, 0x12, 0x01, 0x01, 0x00, 0x00})) ||
      (h.unknown != 0x22)) {
    return false;
  }
  return true;
}

bool isValid(elf64::ElfHeader const& h) {
  if (!std::equal(
          std::begin(h.ident), std::begin(h.ident) + 4,
          std::begin({uint8_t('\x7f'), uint8_t('E'), uint8_t('L'), uint8_t('F'), uint8_t(2), uint8_t(1), uint8_t(1), uint8_t(9), uint8_t(0), uint8_t(2)})) ||
      (h.type != elf64::ET_EXEC && h.type != elf64::ET_DYNEXEC && h.type != elf64::ET_DYNAMIC) || (h.machine != elf64::EM_ARCH_X86_64) || (h.version != 1) ||
      (h.sizePh != sizeof(elf64::ProgramHeader)) || (h.sizeSh > 0 && h.sizeSh != sizeof(elf64::SectionHeader))) {
    return false;
  }
  return true;
}

uint64_t getAlignedSize(elf64::ProgramHeader const* progHeader) {
  return (progHeader->align != 0 ? (progHeader->sizeMem + (progHeader->align - 1)) & ~(progHeader->align - 1) : progHeader->sizeMem);
}

uint64_t calcBaseSize(elf64::ElfHeader const* elfHeader, elf64::ProgramHeader const* progHeader) {
  uint64_t base_size = 0;
  for (size_t i = 0; i < elfHeader->numPh; i++) {
    if (progHeader[i].sizeMem != 0 && (progHeader[i].type == elf64::PH_LOAD || progHeader[i].type == elf64::PH_OS_RELRO)) {
      uint64_t last_addr = progHeader[i].vaddr + getAlignedSize(progHeader + i);
      if (last_addr > base_size) {
        base_size = last_addr;
      }
    }
  }
  return base_size;
}

int getProtection(uint32_t flags) {
  constexpr uint32_t X = (1 << 0); /*Execute*/
  constexpr uint32_t W = (1 << 1); /*Write*/
  constexpr uint32_t R = (1 << 2); /*Read*/

  int retVal = 0;
  if (flags & X) retVal |= SceProtExecute;
  if (flags & W) retVal |= SceProtWrite;
  if (flags & R) retVal |= SceProtRead;

  return retVal;
}

constexpr size_t TRAMP_SIZE = 43;

void createTrampoline64_fs(uint8_t* ptr, int64_t offset, uint64_t addrJmpBack) {
  auto savedPtr = ptr;

  // save regs
  *ptr++ = 0x57; // push rdi
  // - save

  // set param1 (offset)
  *ptr++ = 0x48;
  *ptr++ = 0xB8;

  *((int64_t*)ptr) = offset;
  ptr += 8;

  // call tlsMainGetAddr
  *ptr++ = 0xFF;
  *ptr++ = 0x15;

  auto offsettlsMainGetAddr = (uint64_t)ptr;
  ptr += 4;
  // -

  // restore regs
  *ptr++ = 0x5F; // pop rdi
  // -restore

  // jmp back
  *ptr++ = 0xFF;
  *ptr++ = 0x25;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;

  // set jmp back addr
  *((uint64_t*)ptr) = addrJmpBack;
  ptr += 8;

  // set tlsMainGetAddr and its offset
  *((uint64_t*)ptr)                 = (uint64_t)tlsMainGetAddr64;
  *((int32_t*)offsettlsMainGetAddr) = -4 + (uint64_t)ptr - offsettlsMainGetAddr;
}

void createTrampoline_fs(uint8_t* ptr, uint8_t dstRegister, bool isWork, int32_t offset, uint64_t addrJmpBack) {
  auto savedPtr = ptr;

  // After all the jmps, we end up in this wrapper
  bool const isRax = (isWork && (dstRegister == 0xc0));

  // save regs
  *ptr++ = 0x57;             // push rdi
  if (!isRax) *ptr++ = 0x50; // push rax
  // - save

  // set param1 (offset)
  *ptr++ = 0x48;
  *ptr++ = 0xc7;
  *ptr++ = 0xc7;

  *((int32_t*)ptr) = offset;
  ptr += 4;

  // call tlsMainGetAddr
  *ptr++ = 0xFF;
  *ptr++ = 0x15;

  auto offsettlsMainGetAddr = (uint64_t)ptr;
  ptr += 4;
  // -

  // mov {dstRegister}, rax
  if (!isRax) {
    *ptr++ = isWork ? 0x48 : 0x49;
    *ptr++ = 0x89;
    *ptr++ = dstRegister;
  }
  // -dst

  // restore regs
  if (!isRax) *ptr++ = 0x58; // pop rax
  *ptr++ = 0x5F;             // pop rdi
  // -restore

  // jmp back
  *ptr++ = 0xFF;
  *ptr++ = 0x25;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;

  // set jmp back addr
  *((uint64_t*)ptr) = addrJmpBack;
  ptr += 8;

  // set tlsMainGetAddr and its offset
  *((uint64_t*)ptr)                 = (uint64_t)tlsMainGetAddr;
  *((int32_t*)offsettlsMainGetAddr) = -4 + (uint64_t)ptr - offsettlsMainGetAddr;
  ptr += 8;

  assert((ptr - savedPtr) <= TRAMP_SIZE);
}

uint8_t convReg(uint8_t reg) {
  LOG_USE_MODULE(ELF64);

  switch (reg) {
    case 0x04: return 0xc0;
    case 0x0c: return 0xc1;
    case 0x14: return 0xc2;
    case 0x1c: return 0xc3;
    case 0x24: return 0xc4;
    case 0x2c: return 0xc5;
    case 0x34: return 0xc6;
    case 0x3c: return 0xc7;
    default: LOG_CRIT(L"unknown reg %0x| mov {reg}, fs", reg);
  }
  return 0xc0;
}

void patchProgram(uint64_t slotAddr, uint64_t const patchAddr) {
  uint8_t* ptr = (uint8_t*)patchAddr;

  if (ptr[2] == 0xA1) { // special case: 64 bit offset
    int64_t const  offset      = *((int64_t*)&ptr[3]);
    uint64_t const addrJmpBack = (uint64_t)&ptr[11];

    // Write data to the trampoline data block
    createTrampoline64_fs((uint8_t*)*((uint64_t*)slotAddr), offset, addrJmpBack);

  } else { // 32 bit offset
    uint8_t const  reg         = convReg(ptr[3]);
    bool const     isWork      = ptr[1] == 0x48; // rax ..rdi
    int32_t const  offset      = *((int32_t*)&ptr[5]);
    uint64_t const addrJmpBack = (uint64_t)&ptr[9];

    // Write data to the trampoline data block
    createTrampoline_fs((uint8_t*)*((uint64_t*)slotAddr), reg, isWork, offset, addrJmpBack);
  }

  // create jmp [offset] -> jmp to address
  if (ptr[-1] == 0x66 && ptr[-2] == 0x66) {
    ptr[-3] = 0x90;
    ptr[-2] = 0x90;
    ptr[-1] = 0x90;
  }
  ptr[0] = 0xff;
  ptr[1] = 0x25;

  *((uint32_t*)&ptr[2]) = (int64_t)slotAddr - (int64_t)(ptr + 6);
}

void patchProgram_preprocess(std::list<uint64_t>& patchAddresses, uint64_t const progAddr, uint64_t const vaddr, uint64_t const progSize) {
  //          64 48 8b 04 25 00 00 00 00   mov   rax,QWORD PTR fs:0x0
  // 66 66 66 64 48 8b 04 25 00 00 00 00   mov   rax,QWORD PTR fs:0x0
  //          64 48 8b 0c 25 00 00 00 00   mov   rcx,QWORD PTR fs:0x0
  //          64 4c 8b 04 25 00 00 00 00   mov   r8, QWORD PTR fs:0x0
  // ...

  auto* pStart = (uint8_t*)progAddr;
  auto* pEnd   = (uint8_t*)(progAddr + progSize);

  for (auto* ptr = pStart; ptr < pEnd; ptr++) {
    if (ptr[0] != 0x64) continue;
    if (!(ptr[1] == 0x48 || ptr[1] == 0x4c)) continue;

    // Check for the 64 bit version
    // 64 48 A1 [XXXXXXXXXXXXXXXX]    mov rax,fs:[$XXXXXXXXXXXXXXXX]
    if (ptr[2] == 0xA1) {
      patchAddresses.push_back((uint64_t)(vaddr + ((uint64_t)ptr - progAddr)));
      continue;
    }

    if (ptr[2] != 0x8b) continue;
    if (!(ptr[3] >= 0x04 && ptr[3] <= 0x3c && ((ptr[3] & 0xf) == 0x04 || (ptr[3] & 0xf) == 0xc))) continue;
    if (ptr[4] != 0x25) continue;

    patchAddresses.push_back((uint64_t)(vaddr + ((uint64_t)ptr - progAddr)));
  }
}

elf64::DynamicHeader const* findDynValue(elf64::DynamicHeader const* dynHeaderList, int64_t tag) {
  for (const auto* dyn = dynHeaderList; dyn->tag != 0; ++dyn) {
    if (dyn->tag == tag) {
      return dyn;
    }
  }
  return nullptr;
}

std::string encodeId(uint16_t in_id) {
  std::string ret;

  static const char32_t* str = U"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
  if (in_id < 0x40u) {
    ret += str[in_id];
  } else {
    if (in_id < 0x1000u) {
      ret += str[static_cast<uint16_t>(in_id >> 6u) & 0x3fu];
      ret += str[in_id & 0x3fu];
    } else {
      ret += str[static_cast<uint16_t>(in_id >> 12u) & 0x3fu];
      ret += str[static_cast<uint16_t>(in_id >> 6u) & 0x3fu];
      ret += str[in_id & 0x3fu];
    }
  }
  return ret;
}

unsigned long elfHash(std::string_view name) {
  LOG_USE_MODULE(ELF64);

  unsigned long h = 0, g;
  for (auto item: name) {
    h = (h << 4) + item;
    g = h & 0xf0000000;
    if (g > 0) h ^= g >> 24;
    h &= ~g;
  }
  return h;
}

unsigned long elfHash(std::string_view symName, std::string_view libName, std::string_view modName) {
  LOG_USE_MODULE(ELF64);

  unsigned long h = 0, g;

  auto seperator = [&] {
    h = (h << 4) + '#';
    g = h & 0xf0000000;
    if (g > 0) h ^= g >> 24;
    h &= ~g;
  };

  for (auto item: symName) {
    h = (h << 4) + item;
    g = h & 0xf0000000;
    if (g > 0) h ^= g >> 24;
    h &= ~g;
  }

  seperator();
  for (auto item: libName) {
    h = (h << 4) + item;
    g = h & 0xf0000000;
    if (g > 0) h ^= g >> 24;
    h &= ~g;
  }

  seperator();
  for (auto item: modName) {
    h = (h << 4) + item;
    g = h & 0xf0000000;
    if (g > 0) h ^= g >> 24;
    h &= ~g;
  }

  return h;
}

uint64_t parse_eh_frame_hdr(uint8_t* data) {
  uint8_t const framePtrEncoding = data[1];
  if (framePtrEncoding == elf64::DW_EH_PE_omit) return 0;

  uint64_t addr = 0;
  bool     sign = false;
  switch (framePtrEncoding & 0xF) {
    case elf64::DW_EH_PE_sdata2:
      sign = true;
      addr = (int64_t) * (int16_t*)&data[4];
      break;
    case elf64::DW_EH_PE_udata2: addr = (uint64_t) * (uint16_t*)&data[4]; break;
    case elf64::DW_EH_PE_sdata4: sign = addr = (int64_t) * (int32_t*)&data[4]; break;
    case elf64::DW_EH_PE_udata4: addr = *(uint32_t*)&data[4]; break;
    case elf64::DW_EH_PE_sdata8: addr = (uint64_t) * (int64_t*)&data[4]; break;
    case elf64::DW_EH_PE_udata8: addr = *(uint64_t*)&data[4]; break;
  }

  if ((framePtrEncoding & elf64::DW_EH_PE_absptr) > 0)
    return addr;
  else if ((framePtrEncoding & elf64::DW_EH_PE_datarel) > 0) {
    if (!sign) {
      return 4 + (uint64_t)data[3] + addr;
    }

    return 4 + (uint64_t)data + (int64_t)addr;

  } else if ((framePtrEncoding & elf64::DW_EH_PE_pcrel) > 0)
    assert((framePtrEncoding & elf64::DW_EH_PE_pcrel) > 0);
  return 0;
}

uint64_t parse_eh_frame_size(uint8_t* data) {
  uint8_t const framePtrEncoding = data[1];
  uint8_t const countEncoding    = data[2];
  uint8_t const tableEncoding    = data[3];

  uint8_t* pCounter = 0;

  // get counter offset
  switch (framePtrEncoding & 0xF) {
    case elf64::DW_EH_PE_sdata2:
    case elf64::DW_EH_PE_udata2: pCounter = &data[4 + 2]; break;
    case elf64::DW_EH_PE_sdata4:
    case elf64::DW_EH_PE_udata4: pCounter = &data[4 + 4]; break;
    case elf64::DW_EH_PE_sdata8:
    case elf64::DW_EH_PE_udata8: pCounter = &data[4 + 8]; break;
  }

  uint64_t counter = 0;
  switch (countEncoding & 0xF) {
    case elf64::DW_EH_PE_sdata2:
    case elf64::DW_EH_PE_udata2: counter = *(uint16_t*)pCounter; break;
    case elf64::DW_EH_PE_sdata4:
    case elf64::DW_EH_PE_udata4: counter = *(uint32_t*)pCounter; break;
    case elf64::DW_EH_PE_sdata8:
    case elf64::DW_EH_PE_udata8: counter = *(uint64_t*)pCounter; break;
  }

  switch (tableEncoding & 0xF) {
    case elf64::DW_EH_PE_sdata2:
    case elf64::DW_EH_PE_udata2: counter *= 2 * 2; break;
    case elf64::DW_EH_PE_sdata4:
    case elf64::DW_EH_PE_udata4: counter *= 2 * 4; break;
    case elf64::DW_EH_PE_sdata8:
    case elf64::DW_EH_PE_udata8: counter *= 2 * 8; break;
  }

  return counter;
}
} // namespace

elf64::Elf64_Sym const* elf64::SymbolMap::find(std::string_view fullSymbolname) const {
  auto       nameEnd    = fullSymbolname.find('#');
  auto const symbolName = nameEnd != std::string::npos ? fullSymbolname.substr(0, nameEnd) : fullSymbolname;

  const uint32_t hash = elfHash(fullSymbolname);

  const uint32_t  nbucket = m_hashTable[0];
  const uint32_t  nchain  = m_hashTable[1];
  const uint32_t* bucket  = &m_hashTable[2];
  const uint32_t* chain   = &bucket[nbucket];

  for (uint32_t i = bucket[hash % nbucket]; i; i = chain[i]) {
    auto const res = symbolName.compare(0, symbolName.size(), m_strTable + m_symbolTable[i].name, 0, symbolName.size());
    if (symbolName.compare(0, symbolName.size(), m_strTable + m_symbolTable[i].name, 0, symbolName.size()) == 0) {
      return &m_symbolTable[i];
    }
  }
  return nullptr;
};

elf64::Elf64_Sym const* elf64::SymbolMap::find(std::string_view symbolName, std::string_view libName, std::string_view modName) const {
  const uint32_t hash = elfHash(symbolName, libName, modName);

  const uint32_t  nbucket = m_hashTable[0];
  const uint32_t  nchain  = m_hashTable[1];
  const uint32_t* bucket  = &m_hashTable[2];
  const uint32_t* chain   = &bucket[nbucket];

  for (uint32_t i = bucket[hash % nbucket]; i; i = chain[i]) {
    if (symbolName.compare(0, symbolName.size(), m_strTable + m_symbolTable[i].name, 0, symbolName.size()) == 0) {
      return &m_symbolTable[i];
    }
  }
  return nullptr;
};

class Parser_ELF64: public IFormat {
  std::vector<uint8_t> m_fileData;

  // optional
  std::unique_ptr<elf64::CustomHeader>    m_customHeader;
  std::unique_ptr<elf64::SegmentHeader[]> m_segmentHeader;
  // -

  size_t m_addrElf = 0; /// Offset for all file addresses

  // ###  pointers into m_fileData

  elf64::ElfHeader*     m_elfHeader     = nullptr;
  elf64::ProgramHeader* m_progHeader    = nullptr;
  elf64::SectionHeader* m_sectionHeader = nullptr;

  std::unique_ptr<elf64::DynamicInfo> m_dynamicInfo;

  uint8_t* m_dynamic     = nullptr;
  uint8_t* m_dynamicData = nullptr;
  uint8_t* m_relro       = nullptr;
  uint64_t m_relroVaddr  = 0;

  size_t         m_debugSymTabStride = 0;
  char const*    m_debugStrTab       = nullptr;
  uint64_t       m_debugStrTabSize   = 0;
  uint8_t const* m_debugSymTab       = nullptr;
  uint64_t       m_debugSymTabSize   = 0;

  std::unique_ptr<std::ifstream> m_file;

  public:
  Parser_ELF64(std::filesystem::path&& path, std::unique_ptr<std::ifstream>&& file): IFormat(std::move(path)), m_file(std::move(file)) {}

  bool init() final;
  bool loadSegment(uint8_t* loadAddr, uint64_t fileOffset, uint64_t size) final;
  bool load2Mem(Program* prog) final;
  void getAllocInfo(uint64_t& baseSize, uint64_t& baseSizeAligned, uint64_t& sizeAlloc) final;

  bool containsAddr(uint64_t vaddr, uint64_t baseVaddr) final;

  void setupMissingRelocationHandler(Program* prog, void* relocateHandler, void* payload) final;

  Symbols::SymbolInfo     getSymbolInfo(uint64_t const relIndex) const final;
  Symbols::RelocationInfo getRelocationInfo(IRuntimeLinker const* linker, elf64::Elf64_Rela const* r, Program const* prog,
                                            elf64::SymbolMap const& symbolsMap) const;

  void relocate(Program const* prog, uint64_t invalidMemoryAddr, std::string_view libName) final;

  bool isShared() const { return (m_elfHeader->type == elf64::ET_DYNAMIC); }

  bool isNextGen() const { return (m_elfHeader->ident[elf64::EI_ABIVERSION] == 2); }

  uintptr_t resolve(IRuntimeLinker const& linker, std::string_view const symbolName, Symbols::SymbolType const type, uintptr_t addr) const;

  std::unique_ptr<elf64::DynamicInfo> parseDynamicInfo(std::wstring_view filename, uint64_t baseVaddr) const;

  LibraryId const*       getLibrary(elf64::DynamicInfo const* dynInfo, std::string_view id) const;
  elf64::ModuleId const* getModule(elf64::DynamicInfo const* dynInfo, std::string_view id) const;

  uintptr_t getAddress(std::string_view symName, std::string_view libName, std::string_view modName) const final {
    auto sym = m_dynamicInfo->symbolsMap.find(symName, libName, modName);
    if (sym != nullptr) {
      return m_baseVaddr + sym->value;
    }

    return 0;
  }

  uintptr_t getAddress(std::string_view symName) const final {
    for (auto const& [libName, libId]: m_dynamicInfo->exportedLibs) {
      for (auto const& [modName, modId]: m_dynamicInfo->exportedMods) {
        auto sym = m_dynamicInfo->symbolsMap.find(symName, libName, modName);
        if (sym != nullptr) {
          return m_baseVaddr + sym->value;
        }
      }
    }
    return 0;
  }

  virtual std::unordered_map<std::string_view, LibraryId> const& getExportedLibs() const final { return m_dynamicInfo->exportedLibs; }

  virtual std::unordered_map<std::string_view, LibraryId> const& getImportedLibs() const final { return m_dynamicInfo->importedLibs; }

  virtual std::unordered_map<uint64_t, std::string_view> getDebugStrings() const final {
    std::unordered_map<uint64_t, std::string_view> debugStrings;
    if (m_debugSymTab != nullptr) {
      debugStrings.reserve(m_debugSymTabSize / m_debugSymTabStride);
      for (size_t n = 0; n < m_debugSymTabSize; n += m_debugSymTabStride) {
        elf64::Elf64_Sym const* symbol = (elf64::Elf64_Sym const*)&m_debugSymTab[n];
        if (symbol->name > 0 && symbol->value > 0) {
          std::string_view const symName = (const char*)&m_debugStrTab[symbol->name];
          debugStrings[symbol->value]    = symName;
        }
      }
    }
    return debugStrings;
  }

  void dtInit(uint64_t base, size_t argc, const void* argp) const final {
    LOG_USE_MODULE(ELF64);

    if (m_dynamicInfo->vaddrInit && m_dynamicInfo->vaddrInit.value() != (uint64_t)-1) {
      LOG_DEBUG(L"dt_init(%s) offset:0x%08llx", m_filename.c_str(), m_dynamicInfo->vaddrInit.value());
      jmpModule(base + m_dynamicInfo->vaddrInit.value(), argc, argp, nullptr);
    }

    if (m_dynamicInfo->vaddrInitArray && m_dynamicInfo->sizeInitArray > 0) {
      auto start = (uint64_t const*)(base + m_dynamicInfo->vaddrInitArray.value());
      auto end   = (uint64_t const*)(base + m_dynamicInfo->sizeInitArray + m_dynamicInfo->vaddrInitArray.value());
      for (; start != end; ++start) {
        if (*start != 0 && *start != (uint64_t)-1) {
          LOG_DEBUG(L"initArray(%s) @0x%08llx", m_filename.c_str(), *start);
          callModule(*start);
        }
      }
    }
  };

  void dtDeinit(uint64_t base) const final {
    if (m_dynamicInfo->vaddrFinArray && m_dynamicInfo->sizeFiniArray > 0) {
      auto pStart = (uint64_t const*)(base + m_dynamicInfo->vaddrFinArray.value());
      auto pEnd   = (uint64_t const*)(base + m_dynamicInfo->vaddrFinArray.value() + m_dynamicInfo->sizeFiniArray);
      for (; pStart != pEnd; ++pStart) {
        auto addr = *pStart;
        if (addr != 0 && addr != (uint64_t)-1) callModule(addr);
      }
    }
    if (m_dynamicInfo->vaddrFini) {
      jmpModule(base + m_dynamicInfo->vaddrFini.value(), 0, nullptr, nullptr);
    }
  }

  virtual std::string collectDebugInfos(std::unordered_map<uint64_t, std::string_view>& debugStrings) const final {
    auto        dynInfo = parseDynamicInfo(L"", 0);
    std::string ret;

    if (debugStrings.empty()) debugStrings = getDebugStrings();

    auto const& symMap = dynInfo->symbolsMap;

    // Imports
    std::unordered_map<std::string_view, std::string_view> debugImports;
    std::unordered_map<std::string_view, uint64_t>         debugValues;
    auto                                                   processImports = [&](elf64::Elf64_Rela const* records, uint64_t numBytes) {
      size_t n = 0;
      for (auto const* r = records; reinterpret_cast<uint8_t const*>(r) < reinterpret_cast<uint8_t const*>(records) + numBytes; ++r, ++n) {
        auto const ri = getRelocationInfo(nullptr, r, nullptr, symMap);
        if (ri.name.empty()) continue;

        debugValues[ri.name] = ri.vaddr;
        auto itDebugString   = debugStrings.find(r->offset);
        if (itDebugString != debugStrings.end()) {
          debugImports[ri.name] = itDebugString->second;
        }

        auto idData = util::splitString(ri.name, '#');
        auto lib    = getLibrary(dynInfo.get(), idData[1]);
        auto mod    = getModule(dynInfo.get(), idData[2]);

        ret += std::to_string(ri.vaddr) + "\t";
        ret += "\t-> id:" + std::to_string(n) + " ";
        ret += idData[0];
        ret += "\tlib:";
        ret += lib->name;
        ret += " ver:" + std::to_string(lib->version);
        ret += " mod:";
        ret += mod->name;
        ret += " ver:" + std::to_string(mod->versionMajor) + " " + std::to_string(mod->versionMinor);
        ret += '\n';
      }
    };
    processImports(dynInfo->relaTable, dynInfo->relaTableTotalSize);
    processImports(dynInfo->jmprelaTable, dynInfo->jmprelaTableSize);
    // -

    for (size_t n = 0; n < symMap.size(); ++n) {
      auto const symbol = symMap.getSymbol(n);
      if (symbol.name == 0) continue;

      auto const name = symMap.getName(symbol);

      ret += std::to_string(symbol.value) + "\t";
      auto idData = util::splitString(name, '#');
      if (idData.size() == 3) {
        auto lib = getLibrary(dynInfo.get(), idData[1]);
        auto mod = getModule(dynInfo.get(), idData[2]);
        ret += idData[0];
        ret += "\t\tlib:";
        ret += lib->name;
        ret += " ver:" + std::to_string(lib->version);
        ret += " mod:";
        ret += mod->name;
        ret += " ver:" + std::to_string(mod->versionMajor) + " " + std::to_string(mod->versionMinor);
      } else {
        ret += name;
      }

      if (symbol.value == 0) {
        auto itDebug = debugImports.find(name);
        if (itDebug != debugImports.end()) {
          ret += "|    ";
          ret += itDebug->second;
        }
      } else {
        auto itDebug = debugStrings.find(symbol.value);
        if (itDebug != debugStrings.end()) {
          ret += "|    ";
          ret += itDebug->second;
        }
      }

      ret += '\n';
    }
    return ret;
  }

  void dump(std::fstream& outFile, std::fstream& mapFile);

  std::vector<std::pair<uint64_t, uint64_t>> getExecSections() final {
    std::vector<std::pair<uint64_t, uint64_t>> ret;

    if (m_segmentHeader) {
      for (size_t i = 0; i < m_customHeader->numSegments; ++i) {
        auto const& seg = m_segmentHeader[i];
        if ((seg.type & 0x800u) != 0) {
          auto const  index      = ((seg.type >> 20u) & 0xFFFu);
          auto const& pHeaderCur = m_progHeader[index];
          if ((getProtection(pHeaderCur.flags) & SceProtExecute) > 0) {
            ret.push_back({pHeaderCur.vaddr, pHeaderCur.sizeMem});
            return ret;
          }
        }
      }
    } else {
      for (size_t n = 0; n < m_elfHeader->numSh; ++n) {
        if (m_sectionHeader[n].type == 0x1 && (m_sectionHeader[n].flags & 0x4) > 0) {
          ret.push_back({m_sectionHeader[n].addr, m_sectionHeader[n].size});
        }
      }
    }
    return ret;
  }
};

uintptr_t Parser_ELF64::resolve(IRuntimeLinker const& linker, std::string_view const symbolName, Symbols::SymbolType const type, uintptr_t addr) const {
  LOG_USE_MODULE(ELF64);

  auto idData = util::splitString(symbolName, '#');

  if (idData.size() == 3) {
    auto lib = getLibrary(m_dynamicInfo.get(), idData[1]);
    auto mod = getModule(m_dynamicInfo.get(), idData[2]);
    if (lib != nullptr || mod != nullptr) {

      auto iResolve = linker.getIResolve(lib->name);
      if (iResolve != nullptr) {
        uintptr_t vaddr = iResolve->getAddress(idData[0], lib->name, mod->name);

        if (vaddr != 0) {
          vaddr = linker.checkIntercept(vaddr, idData[0], lib->name, mod->name);
          // LOG_DEBUG(L"Symbol %S vaddr:0x%08llx lib:%S(%d) mod:%S(%d %d) @0x%08llx", symbolName.data(), vaddr, lib->name.data(), lib->version,
          // mod->name.data(),
          //           mod->versionMajor, mod->versionMinor, addr);
          return vaddr;
        }
      }

      // Trace missing function
      if (!__Log::isIgnored(LOG_GET_MODULE(ELF64), __Log::eTrace_Level::trace)) {
        LOG_TRACE(L"Missing: Name:%S @%#08llx Lib:%S(%d) Mod:%S(%d)", symbolName.data(), addr, lib->name.data(), lib->version, mod->name.data(),
                  mod->versionMajor, mod->versionMinor);
      }
      // -
    } else {
      LOG_CRIT(L"lib or mod is null");
    }
  } else {
    // check intern
    auto iResolve = linker.getIResolve("intern");
    if (iResolve != nullptr) {
      uintptr_t vaddr = iResolve->getAddress(idData[0], "intern", "intern");
      return vaddr;
    }
  }

  return 0;
}

Symbols::RelocationInfo Parser_ELF64::getRelocationInfo(IRuntimeLinker const* linker, elf64::Elf64_Rela const* r, Program const* prog,
                                                        elf64::SymbolMap const& symbolsMap) const {
  LOG_USE_MODULE(ELF64);
  Symbols::RelocationInfo ret;

  auto    type   = r->getType();
  auto    symbol = r->getSymbol();
  int64_t addend = r->addend;
  ret.base_vaddr = prog != nullptr ? prog->baseVaddr : 0;
  ret.vaddr      = ret.base_vaddr + r->offset;
  ret.bind_self  = false;

  switch (type) {
    case elf64::R_X86_64_GLOB_DAT:
    case elf64::R_X86_64_JUMP_SLOT: addend = 0; [[fallthrough]];
    case elf64::R_X86_64_64: {
      auto&    sym         = symbolsMap.getSymbol(symbol);
      auto     bind        = sym.getBind();
      auto     sym_type    = sym.getType();
      uint64_t symbolVaddr = 0;

      switch (sym_type) {
        case elf64::STT_NOTYPE: ret.type = Symbols::SymbolType::NoType; break;
        case elf64::STT_FUNC: ret.type = Symbols::SymbolType::Func; break;
        case elf64::STT_OBJECT: ret.type = Symbols::SymbolType::Object; break;
        default: LOG_CRIT(L"unknown symbol type: %d\n", (int)sym_type);
      }
      switch (bind) {
        case elf64::STB_LOCAL:
          symbolVaddr = ret.base_vaddr + sym.value;
          ret.bind    = Symbols::BindType::Local;
          break;
        case elf64::STB_GLOBAL: ret.bind = Symbols::BindType::Global; [[fallthrough]];
        case elf64::STB_WEAK: {
          ret.bind = (ret.bind == Symbols::BindType::Unknown ? Symbols::BindType::Weak : ret.bind);
          ret.name = symbolsMap.getName(sym);

          if (linker != nullptr) symbolVaddr = resolve(*linker, ret.name, ret.type, ret.vaddr);
        } break;
        default: LOG_CRIT(L"unknown bind: %d", (int)bind);
      }
      ret.resolved = (symbolVaddr != 0);
      ret.value    = (ret.resolved ? symbolVaddr + addend : 0);
    } break;
    case elf64::R_X86_64_RELATIVE:
      ret.value    = ret.base_vaddr + addend;
      ret.resolved = true;
      break;
    case elf64::R_X86_64_DTPMOD64: {
      ret.value    = prog != nullptr ? (uint64_t)(prog->tls.index) : 0;
      ret.resolved = true;
      ret.type     = Symbols::SymbolType::TlsModule;
      ret.bind     = Symbols::BindType::Local;
    } break;
    case elf64::R_X86_64_TPOFF64: {
      auto& sym    = symbolsMap.getSymbol(symbol);
      ret.value    = sym.value + r->addend - prog->tls.offset;
      ret.resolved = true;
      ret.type     = Symbols::SymbolType::TlsModule;
      ret.bind     = Symbols::BindType::Local;
    } break;
    default: LOG_CRIT(L"unknown type: %d", (int)type);
  }

  return ret;
}

Symbols::SymbolInfo Parser_ELF64::getSymbolInfo(uint64_t const relIndex) const {

  elf64::Elf64_Rela const* r = m_dynamicInfo->jmprelaTable + relIndex;

  auto const& symbols    = m_dynamicInfo->symbolsMap;
  auto const& sym        = symbols.getSymbol(r->getSymbol());
  auto const  symbolName = symbols.getName(sym);

  auto                idData = util::splitString(symbolName, '#');
  Symbols::SymbolInfo info   = {
        .name = idData[0],
        .type = Symbols::SymbolType::Unknown,
  };

  if (idData.size() == 3) {
    auto lib = getLibrary(m_dynamicInfo.get(), idData[1]);
    auto mod = getModule(m_dynamicInfo.get(), idData[2]);

    if (lib != nullptr) {
      info.libraryName    = lib->name;
      info.libraryVersion = lib->version;
    }
    if (mod != nullptr) {
      info.modulName          = mod->name;
      info.moduleVersionMajor = mod->versionMajor;
      info.moduleVersionMinor = mod->versionMinor;
    }
  }
  return info;
}

LibraryId const* Parser_ELF64::getLibrary(elf64::DynamicInfo const* dynInfo, std::string_view id) const {
  {
    auto& importLibs = dynInfo->importedLibs;

    auto it = std::find_if(importLibs.begin(), importLibs.end(), [id](auto const& lib) { return lib.second.id == id; });
    if (it != importLibs.end()) {
      return &(it->second);
    }
  }
  {
    auto& exportLibs = dynInfo->exportedLibs;

    auto it = std::find_if(exportLibs.begin(), exportLibs.end(), [id](auto const& lib) { return lib.second.id == id; });
    if (it != exportLibs.end()) {
      return &(it->second);
    }
  }
  return nullptr;
}

elf64::ModuleId const* Parser_ELF64::getModule(elf64::DynamicInfo const* dynInfo, std::string_view id) const {
  {
    auto& importedMods = dynInfo->importedMods;

    auto it = std::find_if(importedMods.begin(), importedMods.end(), [id](auto const& lib) { return lib.second.id == id; });
    if (it != importedMods.end()) {
      return &(it->second);
    }
  }
  {
    auto& exportedMods = dynInfo->exportedMods;

    auto it = std::find_if(exportedMods.begin(), exportedMods.end(), [id](auto const& lib) { return lib.second.id == id; });
    if (it != exportedMods.end()) {
      return &(it->second);
    }
  }

  return nullptr;
}

bool Parser_ELF64::containsAddr(uint64_t vaddr, uint64_t baseVaddr) {
  LOG_USE_MODULE(ELF64);
  if (m_intState != util::InterfaceState::Init) {
    LOG_CRIT(L"not init");
    return false;
  }

  for (size_t i = 0; i < m_elfHeader->numPh; i++) {
    if (m_progHeader[i].sizeMem != 0 && (m_progHeader[i].type == elf64::PH_LOAD || m_progHeader[i].type == elf64::PH_OS_RELRO)) {
      uint64_t segment_addr = m_progHeader[i].vaddr + baseVaddr;
      uint64_t segment_size = getAlignedSize(&m_progHeader[i]);

      if (vaddr >= segment_addr && vaddr < segment_addr + segment_size) {
        return true;
      }
    }
  }
  return false;
}

bool Parser_ELF64::loadSegment(uint8_t* loadAddr, uint64_t fileOffset, uint64_t size) {
  memcpy(loadAddr, m_fileData.data() + fileOffset - m_addrElf, size);
  return true;
}

bool Parser_ELF64::init() {
  LOG_USE_MODULE(ELF64);
  [[unlikely]] if (m_intState != util::InterfaceState::NotInit) {
    LOG_CRIT(L"Already init");
    return false;
  }

  util::IntStateErrorOnReturn intStateBomb(m_intState);

  m_fileSize = std::filesystem::file_size(m_path);

  LOG_INFO(L"(%s) size: %u", m_path.c_str(), m_fileSize);

  if ((sizeof(elf64::CustomHeader)) > m_fileSize) {
    LOG_CRIT(L"invalid filesize for (%s)", m_path.c_str());
    return false;
  }

  auto m_customHeader = std::make_unique<elf64::CustomHeader>();

  std::unique_ptr<elf64::SegmentHeader[]> segmentHeader;
  // Check if "custom header" is present
  {
    if (!util::readFile(m_file.get(), m_filename.c_str(), reinterpret_cast<char*>(m_customHeader.get()), sizeof(elf64::CustomHeader))) {
      return false;
    }
    bool valid = isValid(*m_customHeader);
    LOG_DEBUG(L"m_customHeader %s[%S]: s1:%u, s2:%u, sF:%llu, numS:%u, unk:%u, pad:%lu", m_filename.c_str(), util::getBoolStr(valid), m_customHeader->size1,
              m_customHeader->size2, m_customHeader->sizeFile, m_customHeader->numSegments, m_customHeader->unknown, m_customHeader->pad);

    if (!valid) {
      LOG_INFO(L"%s no custom-header", m_filename.c_str());
      m_customHeader.reset();
      m_file->seekg(0);
    } else {
      // Parse SegmentHeader
      m_segmentHeader = std::make_unique<elf64::SegmentHeader[]>(m_customHeader->numSegments);
      if (!util::readFile(m_file.get(), m_filename.c_str(), reinterpret_cast<char*>(m_segmentHeader.get()),
                          sizeof(elf64::SegmentHeader) * m_customHeader->numSegments)) {
        return false;
      }

      for (size_t n = 0; n < m_customHeader->numSegments; ++n) {
        LOG_DEBUG(L"segHeader[%lu] %s: type:0x%08llx, offset:0x%08llx, sizeCom:%llu, "
                  L"sizeDecom:%llu",
                  n, m_filename.c_str(), m_segmentHeader[n].type, m_segmentHeader[n].offset, m_segmentHeader[n].sizeCom, m_segmentHeader[n].sizeDecom);
      }
      // -
    }
  }
  // - Check custom header

  // Parse ELF Header
  m_addrElf               = m_file->tellg();
  uint64_t actualFileSize = m_fileSize;

  // Get actual file size after decompress etc (needed if customheader available)
  {
    LOG_TRACE(L"Address ElfHeader %s: 0x%08llx", m_filename.c_str(), m_addrElf);
    auto elfHeader = std::make_unique<elf64::ElfHeader>();

    if (!util::readFile(m_file.get(), m_filename.c_str(), reinterpret_cast<char*>(elfHeader.get()), sizeof(elf64::ElfHeader))) {
      return false;
    }
    bool valid = isValid(*elfHeader);

    LOG_TRACE(L"ElfHeader %s[%S]: type:0x%02x, machine:0x%0x, ver:%lu, entry:0x%08llx, "
              L"phOff:0x%08llx, shOff:0x%08llx, flags:%lu, sizeEh:%u, sizePh:%u, "
              L"numPh:%u, sizeSH:%u, numSH:%u, indexSh:%u ",
              m_filename.c_str(), util::getBoolStr(valid), elfHeader->type, elfHeader->machine, elfHeader->version, elfHeader->entry, elfHeader->phOff,
              elfHeader->shOff, elfHeader->flags, elfHeader->sizeEh, elfHeader->sizePh, elfHeader->numPh, elfHeader->sizeSh, elfHeader->numSh,
              elfHeader->indexSh);
    if (!valid) {
      LOG_CRIT(L"elfHeader not valid %s: size(PH):%lu, size(SH):%lu, ident: %0x%0x%0x%0x ", m_filename.c_str(), sizeof(elf64::ProgramHeader),
               sizeof(elf64::SectionHeader), elfHeader->ident[0], elfHeader->ident[1], elfHeader->ident[2], elfHeader->ident[3]);
      return false;
    }
    // - Parse ELF Header

    // Parse ProgramHeader
    m_file->seekg(m_addrElf + elfHeader->phOff);
    auto progHeader = std::make_unique<elf64::ProgramHeader[]>(elfHeader->numPh);
    if (!util::readFile(m_file.get(), m_filename.c_str(), reinterpret_cast<char*>(progHeader.get()), sizeof(elf64::ProgramHeader) * elfHeader->numPh)) {
      return false;
    }

    for (size_t n = 0; n < elfHeader->numPh; ++n) {
      auto&          item    = progHeader[n];
      uint64_t const endAddr = item.offset + item.sizeFile;

      if (actualFileSize < endAddr) actualFileSize = endAddr;

      LOG_DEBUG(L"item[%lu] %s: type:0x%04x, flags:%u, offset:0x%08llx, "
                L"vaddr:0x%08llx, paddr:0x%08llx, sizeFile:%llu, sizeMem:%llu, "
                L"align:%llu",
                n, m_filename.c_str(), item.type, item.flags, item.offset, item.vaddr, item.paddr, item.sizeFile, item.sizeMem, item.align);
    }
  }
  // - ProgramHeader

  // Read file and decompress etc if needed (custom segments)
  m_file->seekg(m_addrElf, std::ios_base::beg);
  m_fileData.resize(actualFileSize - m_addrElf);
  if (!util::readFile(m_file.get(), m_filename.c_str(), (char*)m_fileData.data(), m_fileSize - m_addrElf)) {
    return false;
  }

  // set pointers
  m_elfHeader  = (elf64::ElfHeader*)&m_fileData[0];
  m_progHeader = (elf64::ProgramHeader*)&m_fileData[m_elfHeader->phOff];

  // Section Header
  if (m_elfHeader->numSh > 0 && m_elfHeader->shOff < m_fileData.size() && (elf64::SectionHeader*)&m_fileData[m_elfHeader->shOff]) {
    m_sectionHeader = (elf64::SectionHeader*)&m_fileData[m_elfHeader->shOff];

    // Strings available -> load
    if (m_sectionHeader->size > 0 && !__Log::isIgnored(LOG_GET_MODULE(ELF64), __Log::eTrace_Level::trace)) {
      char const* debugData = (char const*)(m_fileData.data() + m_sectionHeader[m_elfHeader->indexSh].offset); // todo test if offset is needed

      for (size_t n = 0; n < m_elfHeader->numSh; ++n) {
        auto const&            header      = m_sectionHeader[n];
        std::string_view const sectionName = debugData + m_sectionHeader[n].name;

        if (m_sectionHeader[n].type == 0x2 && sectionName.compare(".symtab") == 0) {
          m_debugSymTabStride = header.entrySize;
          m_debugSymTab       = m_fileData.data() + header.offset; // todo test if offset is needed
          m_debugSymTabSize   = header.size;
        } else if (m_sectionHeader[n].type == 0x3 && sectionName.compare(".strtab") == 0) {
          m_debugStrTab     = (char const*)(m_fileData.data() + header.offset); // todo test if offset is needed
          m_debugStrTabSize = header.size;
        }

        LOG_DEBUG(L"sectionHeader %s: name:%S, type:0x%04x, flags:%u, "
                  L"addr:0x%08llx, "
                  L"offset:0x%08llx, size:0x%08llx, link:%lu, info:%lu, addAlign:%llu, "
                  L"entrySize:%llu",
                  m_filename.c_str(), debugData + header.name, header.type, header.flags, header.addr, header.offset, header.size, header.link, header.info,
                  header.addrAlign, header.entrySize);
      }
    }
    // - Section Header
  }
  // - set pointers

  // segment unpack
  if (m_customHeader) {
    for (size_t i = 0; i < m_customHeader->numSegments; ++i) {
      auto const& segHeader = m_segmentHeader[i];
      if ((segHeader.type & elf64::SF_BFLG) != 0) {
        assert((segHeader.type & elf64::SF_ENCR) == 0);
        assert((segHeader.type & elf64::SF_DFLG) == 0);
        assert(segHeader.sizeCom == segHeader.sizeDecom);

        auto const  index      = ((segHeader.type >> 20u) & 0xFFFu);
        auto const& progHeader = m_progHeader[index];

        uint8_t* dst = &m_fileData[progHeader.offset - m_addrElf];

        // Read from file (m_fileData may already be overwritten by prev calls)
        m_file->seekg(segHeader.offset, std::ios_base::beg);
        if (!util::readFile(m_file.get(), m_filename.c_str(), (char*)dst, segHeader.sizeDecom)) {
          return false;
        }
      }
    }
  }
  // -

  intStateBomb.defuse();
  m_intState = util::InterfaceState::Init;
  LOG_INFO(L"<-- init(%s)", m_filename.c_str());

  return true;
}

void Parser_ELF64::getAllocInfo(uint64_t& baseSize, uint64_t& baseSizeAligned, uint64_t& sizeAlloc) {
  baseSize        = calcBaseSize(m_elfHeader, m_progHeader);
  baseSizeAligned = (baseSize & ~(static_cast<uint64_t>(0x1000) - 1)) + 0x1000;
  sizeAlloc       = baseSizeAligned + ExceptionHandler::getAllocSize();
}

bool Parser_ELF64::load2Mem(Program* prog) {
  LOG_USE_MODULE(ELF64);
  if (m_intState != util::InterfaceState::Init) {
    LOG_CRIT(L"Not Init");
    return false;
  }

  std::list<uint64_t> fsPatchAddrList;

  // ### Preparse patching
  for (size_t n = 0; n < m_elfHeader->numPh; ++n) {
    auto& progHeader = m_progHeader[n];

    if ((progHeader.type == elf64::PH_LOAD || progHeader.type == elf64::PH_OS_RELRO) && progHeader.sizeMem != 0 && progHeader.flags != 0) {
      auto const mode = getProtection(progHeader.flags);

      if (memory::isExecute(mode)) {
        patchProgram_preprocess(fsPatchAddrList, (uint64_t)(m_fileData.data() + progHeader.offset - m_addrElf), progHeader.vaddr, progHeader.sizeFile);
      }
    }
  }
  // ---

  bool const shared = isShared();
  // bool const nextGen = isNextGen();

  prog->baseVaddr = memory::alloc(prog->desiredBaseAddr, prog->baseSizeAligned + /*fs_table: */ sizeof(uint64_t) * (1 + fsPatchAddrList.size()),
                                  SceProtExecute | SceProtRead | SceProtWrite);

  printf("[%d] %S| Image allocation, addr:0x%08llx size:0x%08llx\n", prog->id, m_filename.c_str(), prog->baseVaddr, prog->allocSize);
  LOG_DEBUG(L"[%d] %s| Image allocation, addr:0x%08llx(0x%08llx) size:0x%08llx", prog->id, m_filename.c_str(), prog->baseVaddr, prog->desiredBaseAddr,
            prog->allocSize);

  m_baseVaddr = prog->baseVaddr;
  ExceptionHandler::install(prog->baseVaddr, (prog->baseVaddr + prog->baseSizeAligned) - ExceptionHandler::getAllocSize(), prog->baseSize);

  strcpy_s(prog->moduleInfoEx.name, prog->filename.string().c_str());
  prog->moduleInfoEx.id = prog->id;

  // Load Segments
  for (size_t n = 0; n < m_elfHeader->numPh; ++n) {
    auto& progHeader = m_progHeader[n];

    if (n < 3) {
      auto& seg   = prog->moduleInfoEx.segments[n];
      seg.address = progHeader.vaddr + prog->baseVaddr;
      seg.prot    = progHeader.flags;
      seg.size    = progHeader.sizeMem;

      prog->moduleInfoEx.segment_count = 1 + n;
    }

    // Load data to virtual
    if ((progHeader.type == elf64::PH_LOAD || progHeader.type == elf64::PH_OS_RELRO) && progHeader.sizeMem != 0 && progHeader.flags != 0) {
      auto const     mode       = getProtection(progHeader.flags);
      uint64_t const addr       = util::alignDown(progHeader.vaddr + prog->baseVaddr, progHeader.align);
      uint64_t const sizeMemory = getAlignedSize(&progHeader);

      LOG_INFO(L"%s| LoadSegment[%u]: type:0x%08x addr:0x%08llx size:0x%08llx(align:0x%08llx) mode:0x%x", prog->filename.c_str(), n, progHeader.type, addr,
               progHeader.sizeMem, sizeMemory, mode);
      if (!loadSegment((uint8_t*)addr, progHeader.offset, progHeader.sizeFile)) {
        LOG_CRIT(L"loadSegment error (%s)", prog->filename.c_str());
      }
      memset((uint8_t*)(addr + progHeader.sizeFile), 0, sizeMemory - progHeader.sizeFile); // fill if needed
    }
    // -load virtual

    if (progHeader.type == elf64::PH_GNU_EH_HEADER) {
      prog->moduleInfoEx.eh_frame_hdr_addr = (void*)(progHeader.vaddr + prog->baseVaddr);
      prog->moduleInfoEx.eh_frame_addr     = (void*)parse_eh_frame_hdr((uint8_t*)prog->moduleInfoEx.eh_frame_hdr_addr);
      prog->moduleInfoEx.eh_frame_size     = parse_eh_frame_size((uint8_t*)prog->moduleInfoEx.eh_frame_hdr_addr);

      LOG_DEBUG(L"%s| EH_FRAME_HEADER[%u] addr:0x%08llx frame@0x%08llx size:0x%08llx", prog->filename.c_str(), n, prog->moduleInfoEx.eh_frame_hdr_addr,
                prog->moduleInfoEx.eh_frame_addr, prog->moduleInfoEx.eh_frame_size);
    } else if (progHeader.type == elf64::PH_TLS) {
      assert(progHeader.vaddr < prog->baseSize);

      prog->tls.vaddrImage = progHeader.vaddr + prog->baseVaddr;
      prog->tls.sizeImage  = progHeader.sizeMem;
      prog->tls.alignment  = progHeader.align;

      prog->moduleInfoEx.tls_init_addr = (void*)(progHeader.vaddr + prog->baseVaddr);
      prog->moduleInfoEx.tls_init_size = progHeader.sizeFile;
      prog->moduleInfoEx.tls_size      = progHeader.sizeMem;
      prog->moduleInfoEx.tls_align     = progHeader.align;

      LOG_DEBUG(L"%s| TLS[%u] addr:0x%08llx size:0x%08llx", prog->filename.c_str(), n, prog->tls.vaddrImage, prog->tls.sizeImage);
    } else if (progHeader.type == elf64::PH_OS_PROCPARAM) {
      assert(prog->procParamVaddr == 0);
      assert(progHeader.vaddr < prog->baseSize);

      prog->procParamVaddr = progHeader.vaddr + prog->baseVaddr;
    } else if (progHeader.type == elf64::PH_DYNAMIC) {
      m_dynamic = (uint8_t*)(m_fileData.data() + progHeader.offset - m_addrElf);
      LOG_DEBUG(L"%s| Dynamic[%u] addr:0x%08llx, size:0x%08llx", m_filename.c_str(), n, (uint64_t)m_dynamic, progHeader.sizeFile);
    } else if (progHeader.type == elf64::PH_OS_DYNLIBDATA) {
      m_dynamicData = (uint8_t*)(m_fileData.data() + progHeader.offset - m_addrElf);
      LOG_DEBUG(L"%s| DynamicData[%u] addr:0x%08llx, size:0x%08llx", m_filename.c_str(), n, (uint64_t)m_dynamicData, progHeader.sizeFile);
    } else if (progHeader.type == elf64::PH_OS_RELRO) {
      m_relro      = (uint8_t*)(m_fileData.data() + progHeader.offset - m_addrElf);
      m_relroVaddr = progHeader.vaddr + prog->baseVaddr;
      LOG_DEBUG(L"%s| relro[%u] addr:0x%08llx, size:0x%08llx", m_filename.c_str(), n, (uint64_t)m_dynamicData, progHeader.sizeFile);
    }
  }
  // -

  // ### PATCHING
  // patch fs
  if (!fsPatchAddrList.empty()) {
    uint64_t const addrFsList = util::alignUp(prog->baseVaddr + prog->baseSizeAligned, 64);
    prog->trampolines_fs.resize(TRAMP_SIZE * fsPatchAddrList.size());

    LOG_DEBUG(L"tramp(fs) table:0x%08llx data:0x%08llx", addrFsList, (uint64_t)prog->trampolines_fs.data());

    size_t n = 0;

    auto trampSlot   = prog->trampolines_fs.data();
    auto pAddrFsList = (uint64_t*)addrFsList;

    for (auto const& fsCall: fsPatchAddrList) {
      *pAddrFsList = (uint64_t)trampSlot; // put addr in table
      patchProgram((uint64_t)pAddrFsList, prog->baseVaddr + fsCall);

      ++pAddrFsList;
      trampSlot += TRAMP_SIZE;
      ++n;
    }

    memory::protect((uint64_t)prog->trampolines_fs.data(), prog->trampolines_fs.size(),
                    SceProtExecute | SceProtRead | SceProtWrite); // Protecting heap! Ok, just adding exec
    flushInstructionCache((uint64_t)prog->trampolines_fs.data(), prog->trampolines_fs.size());
  }
  // --- PATCHING

  // Set the protection
  for (size_t n = 0; n < m_elfHeader->numPh; ++n) {
    auto& progHeader = m_progHeader[n];

    if ((progHeader.type == elf64::PH_LOAD || progHeader.type == elf64::PH_OS_RELRO) && progHeader.sizeMem != 0 && progHeader.flags != 0) {
      auto const     mode       = getProtection(progHeader.flags);
      uint64_t const addr       = util::alignDown(progHeader.vaddr + prog->baseVaddr, progHeader.align);
      uint64_t const sizeMemory = getAlignedSize(&progHeader);

      memory::protect(addr, sizeMemory, mode); // Protect Memory
      flushInstructionCache(addr, sizeMemory);
    }
  }
  // - protect

  // Is main program?
  if (!shared) {
    prog->isMainProg = true;
  }

  m_dynamicInfo = parseDynamicInfo(prog->filename.c_str(), prog->baseVaddr);
  if (m_dynamicInfo != nullptr) {
    prog->entryOffAddr = m_elfHeader->entry;
    if (m_dynamicInfo->vaddrInit) prog->moduleInfoEx.init_proc_addr = *m_dynamicInfo->vaddrInit;
    if (m_dynamicInfo->vaddrFini) prog->moduleInfoEx.fini_proc_addr = *m_dynamicInfo->vaddrFini;
  }
  LOG_INFO(L"<-- load2Mem %s", m_filename.c_str());

  return true;
}

std::unique_ptr<elf64::DynamicInfo> Parser_ELF64::parseDynamicInfo(std::wstring_view filename, uint64_t baseVaddr) const {
  LOG_USE_MODULE(ELF64);
  LOG_TRACE(L"--> parseDynamicInfo %s", filename.data());

  if (m_dynamicData == nullptr) {
    LOG_WARN(L"%s no dynamicData", filename.data());
    return nullptr;
  }

  // find dynamicData by tag
  auto getDynamicData = [this, baseVaddr](auto const tagOs, auto const tag) -> elf64::DynamicHeader const* {
    if (const auto* dyn = findDynValue(reinterpret_cast<elf64::DynamicHeader*>(m_dynamic), tagOs); dyn != nullptr) {
      return reinterpret_cast<elf64::DynamicHeader*>(m_dynamicData + dyn->_un.ptr);
    } else if (const auto* dyn = findDynValue(reinterpret_cast<elf64::DynamicHeader*>(m_dynamic), tag); dyn != nullptr) {
      return reinterpret_cast<elf64::DynamicHeader*>(baseVaddr + dyn->_un.ptr);
    }
    return nullptr;
  };

  auto getDynamicDataPtr = [this](auto const tag) -> uint64_t {
    if (const auto* dyn = findDynValue(reinterpret_cast<elf64::DynamicHeader*>(m_dynamic), tag); dyn != nullptr) {
      return dyn->_un.ptr;
    }
    return NULL;
  };

  auto getDynamicDataValue = [this](auto const tag1, auto const tag2) -> uint64_t {
    uint64_t ret = 0;
    if (tag1 != 0) {
      if (const auto* dyn = findDynValue(reinterpret_cast<elf64::DynamicHeader*>(m_dynamic), tag1); dyn != nullptr) {
        ret = dyn->_un.value;
      }
    }

    if (ret == 0 && tag2 != 0) {
      if (const auto* dyn = findDynValue(reinterpret_cast<elf64::DynamicHeader*>(m_dynamic), tag2); dyn != nullptr) {
        ret = dyn->_un.value;
      }
    }
    return ret;
  };

  auto getLibs = [this](char const* names, auto const tag1, auto const tag2) {
    std::unordered_map<std::string_view, LibraryId> libs;

    for (const auto* dyn = reinterpret_cast<elf64::DynamicHeader*>(m_dynamic); dyn->tag != 0; ++dyn) {
      if (dyn->tag == tag1 || dyn->tag == tag2) {
        LibraryId lib {
            .id      = encodeId(static_cast<uint16_t>((dyn->_un.value >> 48u) & 0xffffu)),
            .version = static_cast<int>((dyn->_un.value >> 32u) & 0xffffu),
            .name    = names + (dyn->_un.value & 0xffffffff),
        };
        libs.insert({lib.name, std::move(lib)});
      }
    }
    return libs;
  };

  auto getMods = [this](char const* names, auto const tag1, auto const tag2) {
    std::unordered_map<std::string_view, elf64::ModuleId> mods;

    for (const auto* dyn = reinterpret_cast<elf64::DynamicHeader*>(m_dynamic); dyn->tag != 0; ++dyn) {
      if (dyn->tag == tag1 || dyn->tag == tag2) {
        std::string_view modName = names + (dyn->_un.value & 0xffffffff);

        mods.insert({modName, elf64::ModuleId {
                                  .id           = encodeId(static_cast<uint16_t>((dyn->_un.value >> 48u) & 0xffffu)),
                                  .versionMajor = static_cast<int>((dyn->_un.value >> 40u) & 0xffu),
                                  .versionMinor = static_cast<int>((dyn->_un.value >> 32u) & 0xffu),
                                  .name         = modName,
                              }});
      }
    }
    return mods;
  };

  auto const hashTable     = (const uint32_t*)getDynamicData(elf64::DT_OS_HASH, elf64::DT_HASH);
  auto const hashTableSize = getDynamicDataValue(elf64::DT_OS_HASHSZ, 0);
  LOG_DEBUG(L"(%s) hashTable addr:%#08llx size:%llu", filename.data(), reinterpret_cast<uint64_t>(hashTable), hashTableSize);

  auto const strTable     = (const char*)getDynamicData(elf64::DT_OS_STRTAB, elf64::DT_STRTAB);
  auto const strTableSize = getDynamicDataValue(elf64::DT_OS_STRSZ, elf64::DT_STRSZ);
  LOG_DEBUG(L"(%s) strTable addr:%#08llx size:%llu", filename.data(), (uint64_t)(strTable), strTableSize);

  auto const symbolTable          = (elf64::Elf64_Sym const*)getDynamicData(elf64::DT_OS_SYMTAB, elf64::DT_SYMTAB);
  auto const symbolTableSizeTotal = getDynamicDataValue(elf64::DT_OS_SYMTABSZ, 0);
  auto const symbolTableSizeEntry = getDynamicDataValue(elf64::DT_OS_SYMENT, elf64::DT_SYMENT);
  LOG_DEBUG(L"(%s) symbolTable addr:%#08llx size:%llu sizeEntry%llu", filename.data(), reinterpret_cast<uint64_t>(symbolTable), symbolTableSizeTotal,
            symbolTableSizeEntry);

  auto dynInfo = std::make_unique<elf64::DynamicInfo>(elf64::SymbolMap(hashTable, hashTableSize, strTable, strTableSize, symbolTable,
                                                                       symbolTable != nullptr ? symbolTableSizeTotal / symbolTableSizeEntry : 0));

  dynInfo->vaddrInit = getDynamicDataPtr(elf64::DT_INIT);
  dynInfo->vaddrFini = getDynamicDataPtr(elf64::DT_FINI);

  dynInfo->vaddrInitArray    = getDynamicDataPtr(elf64::DT_INIT_ARRAY);
  dynInfo->vaddrFinArray     = getDynamicDataPtr(elf64::DT_FINI_ARRAY);
  dynInfo->vaddrPreinitArray = getDynamicDataPtr(elf64::DT_PREINIT_ARRAY);
  dynInfo->sizePreinitArray  = getDynamicDataPtr(elf64::DT_PREINIT_ARRAYSZ);
  dynInfo->sizeInitArray     = getDynamicDataPtr(elf64::DT_INIT_ARRAYSZ);
  dynInfo->sizeFiniArray     = getDynamicDataPtr(elf64::DT_FINI_ARRAYSZ);
  dynInfo->vaddrPltgot       = getDynamicDataPtr(elf64::DT_OS_PLTGOT);
  if (dynInfo->vaddrPltgot == 0) dynInfo->vaddrPltgot = getDynamicDataPtr(elf64::DT_PLTGOT);

  if (getDynamicDataValue(elf64::DT_OS_PLTREL, elf64::DT_PLTREL) == elf64::DT_RELA) {
    dynInfo->jmprelaTable     = (elf64::Elf64_Rela const*)getDynamicData(elf64::DT_OS_JMPREL, elf64::DT_JMPREL);
    dynInfo->jmprelaTableSize = getDynamicDataValue(elf64::DT_OS_PLTRELSZ, elf64::DT_PLTRELSZ);
    LOG_DEBUG(L"(%s) jmprelaTable addr:%#08llx size:%llu", filename.data(), (uint64_t)(dynInfo->jmprelaTable), dynInfo->jmprelaTableSize);
  }

  dynInfo->relaTable          = (elf64::Elf64_Rela const*)getDynamicData(elf64::DT_OS_RELA, elf64::DT_RELA);
  dynInfo->relaTableTotalSize = getDynamicDataValue(elf64::DT_OS_RELASZ, elf64::DT_RELASZ);
  dynInfo->relaTableEntrySize = getDynamicDataValue(elf64::DT_OS_RELAENT, elf64::DT_RELAENT);
  LOG_DEBUG(L"(%s) relaTable %#08llx size:%llu entrysize:%llu", filename.data(), (uint64_t)(dynInfo->relaTable), dynInfo->relaTableTotalSize,
            dynInfo->relaTableEntrySize);

  dynInfo->relativeCount = getDynamicDataValue(elf64::DT_RELACOUNT, 0);
  dynInfo->debug         = getDynamicDataValue(elf64::DT_DEBUG, 0);
  dynInfo->flags         = getDynamicDataValue(elf64::DT_FLAGS, 0);
  dynInfo->textrel       = getDynamicDataValue(elf64::DT_TEXTREL, 0);
  LOG_DEBUG(L"(%s) relativeCount:%08llx, debug:%08llx, flags:%08llx, textrel:%08llx", filename.data(), dynInfo->relativeCount, dynInfo->debug, dynInfo->flags,
            dynInfo->textrel);

  // ### Libs
  dynInfo->importedLibs = getLibs(strTable, elf64::DT_OS_IMPORT_LIB, elf64::DT_OS_IMPORT_LIB_1);
  dynInfo->exportedLibs = getLibs(strTable, elf64::DT_OS_EXPORT_LIB, elf64::DT_OS_EXPORT_LIB_1);

  // ### Modules
  dynInfo->importedMods = getMods(strTable, elf64::DT_OS_NEEDED_MODULE, elf64::DT_OS_NEEDED_MODULE_1);
  dynInfo->exportedMods = getMods(strTable, elf64::DT_OS_MODULE_INFO, elf64::DT_OS_MODULE_INFO_1);

  LOG_INFO(L"(%s) Libs import:%llu export:%llu", filename.data(), dynInfo->importedLibs.size(), dynInfo->exportedLibs.size());

  LOG_TRACE(L"<-- parseDynamicInfo %s", filename.data());
  return dynInfo;
}

void Parser_ELF64::setupMissingRelocationHandler(Program* prog, void* relocateHandler, void* payload) {
  uint64_t const pltgot_vaddr = m_dynamicInfo->vaddrPltgot + prog->baseVaddr;
  uint64_t const pltgot_size  = static_cast<uint64_t>(3) * 8;
  void const**   pltgot       = reinterpret_cast<void const**>(pltgot_vaddr);

  int oldMode {};
  memory::protect(pltgot_vaddr, pltgot_size, SceProtWrite, &oldMode);

  pltgot[1] = payload;
  pltgot[2] = relocateHandler;

  memory::protect(pltgot_vaddr, pltgot_size, oldMode);

  if (memory::isExecute(oldMode)) {
    flushInstructionCache(pltgot_vaddr, pltgot_size);
  }

  if (prog->pltVaddr == 0) {
    prog->pltNum   = m_dynamicInfo->relaTableTotalSize / sizeof(elf64::Elf64_Rela);
    auto size      = CallPlt::getSize(prog->pltNum);
    prog->pltVaddr = memory::alloc(SYSTEM_RESERVED, size, SceProtWrite);

    auto* code = new (reinterpret_cast<void*>(prog->pltVaddr)) CallPlt(prog->pltNum);
    code->setPltGot(pltgot_vaddr);
    memory::protect(prog->pltVaddr, size, SceProtExecute);
    flushInstructionCache(prog->pltVaddr, size);
  }
}

void Parser_ELF64::relocate(Program const* prog, uint64_t invalidMemoryAddr, std::string_view libName) {
  LOG_USE_MODULE(ELF64);
  LOG_INFO(L"relocate %s", prog->filename.c_str());

  auto load = [this](uint64_t const offset) -> uint64_t {
    uint64_t    ret = 0;
    auto const* src = m_relro + (offset - m_relroVaddr);
    memcpy((uint8_t*)&ret, src, sizeof(ret));
    return ret;
  };

  auto relocateAll = [&](IRuntimeLinker& linker, elf64::Elf64_Rela const* records, uint64_t numBytes, bool const isJmpRelaTable) {
    uint32_t index = 0;
    for (auto const* r = records; reinterpret_cast<uint8_t const*>(r) < reinterpret_cast<uint8_t const*>(records) + numBytes; r++, index++) {
      auto const ri = getRelocationInfo(&linker, r, prog, m_dynamicInfo->symbolsMap);

      if (!libName.empty()) {
        auto idData = util::splitString(ri.name, '#');

        if (idData.size() == 3) {
          auto lib = getLibrary(m_dynamicInfo.get(), idData[1]);
          if (lib->name != libName) continue;
        } else
          continue;
      }

      bool patched = false;
      if (ri.resolved) {
        patched = patchReplace(ri.vaddr, ri.value);
      } else {
        uint64_t   value = 0;
        bool const weak  = (ri.bind == Symbols::BindType::Weak || !prog->failGlobalUnresolved);

        if (ri.type == Symbols::SymbolType::Object && weak) {
          value = invalidMemoryAddr;
        } else if (ri.type == Symbols::SymbolType::Func && isJmpRelaTable) {
          if (prog->pltVaddr != 0) {
            value = reinterpret_cast<CallPlt*>(prog->pltVaddr)->getAddr(index);
          } else {
            value = load(ri.vaddr) + ri.base_vaddr;
          }
        } else if ((ri.type == Symbols::SymbolType::Func && !isJmpRelaTable) || (ri.type == Symbols::SymbolType::NoType && weak)) {
          value = load(ri.vaddr) + ri.base_vaddr;
        }

        if (value != 0) {
          patched = patchReplace(ri.vaddr, value);
        } else {
          LOG_USE_MODULE(ELF64);
          LOG_CRIT(L"Can't resolve(%s): [0x%08llx] <- 0x%08llx %S, %S, %S, rela:%d weak:%d test:0x%08llx", prog->filename.c_str(), ri.vaddr, ri.value,
                   ri.name.data(), magic_enum::enum_name(ri.type).data(), magic_enum::enum_name(ri.bind).data(), isJmpRelaTable, weak, load(ri.vaddr));
        }
      }
    }
  };

  auto& linker = accessRuntimeLinker();
  relocateAll(linker, m_dynamicInfo->relaTable, m_dynamicInfo->relaTableTotalSize, false);
  relocateAll(linker, m_dynamicInfo->jmprelaTable, m_dynamicInfo->jmprelaTableSize, true);
}

std::unique_ptr<IFormat> buildParser_Elf64(std::filesystem::path&& path, std::unique_ptr<std::ifstream>&& file) {
  auto inst = std::make_unique<Parser_ELF64>(std::move(path), std::move(file));

  return std::unique_ptr<IFormat>(inst.release());
}

void Parser_ELF64::dump(std::fstream& outFile, std::fstream& mapFile) {
  LOG_USE_MODULE(ELF64);

  // Copy elf part
  m_file->seekg(m_addrElf);
  outFile << m_file->rdbuf();
  // -

  for (size_t i = 0; i < m_customHeader->numSegments; ++i) {
    auto const& seg = m_segmentHeader[i];
    if ((seg.type & 0x800u) != 0) {
      auto const  index      = ((seg.type >> 20u) & 0xFFFu);
      auto const& pHeaderCur = m_progHeader[index];
      if (pHeaderCur.type == elf64::PH_LOAD || pHeaderCur.type == elf64::PH_OS_RELRO) {
        m_file->seekg(seg.offset);
        outFile.seekg(pHeaderCur.offset);
        std::vector<char> buffer(seg.sizeDecom);
        m_file->read(buffer.data(), buffer.size());
        outFile.write(buffer.data(), buffer.size());

        LOG_INFO(L"copy [%llu %llu] @0x%08llx| 0x%08llx-> 0x%08llx", i, index, pHeaderCur.offset, seg.offset, pHeaderCur.offset);
      }
    }
  }

  // Map file
  if (!m_dynamicInfo) m_dynamicInfo = parseDynamicInfo(m_filename.c_str(), 0);
  std::string mappings;
  auto        getSymbols = [&](elf64::Elf64_Rela const* records, uint64_t numBytes) {
    for (auto const* r = records; reinterpret_cast<uint8_t const*>(r) < reinterpret_cast<uint8_t const*>(records) + numBytes; r++) {
      auto& sym  = m_dynamicInfo->symbolsMap.getSymbol(r->getSymbol());
      auto  name = m_dynamicInfo->symbolsMap.getName(sym);

      if (!name.empty()) {
        auto idData = util::splitString(name, '#');
        mappings += std::format("{} {:#x}\n", std::string(idData[0].data(), idData[0].size()).data(), r->offset);
      }
    }
  };

  getSymbols(m_dynamicInfo->relaTable, m_dynamicInfo->relaTableTotalSize);
  getSymbols(m_dynamicInfo->jmprelaTable, m_dynamicInfo->jmprelaTableSize);

  mapFile.write(mappings.data(), mappings.size());
}

void elf64::dump(IFormat* format, std::fstream& outFile, std::fstream& mapFile) {
  ((Parser_ELF64*)format)->dump(outFile, mapFile);
}
