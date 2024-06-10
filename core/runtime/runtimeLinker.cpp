#define __APICALL_EXTERN
#include "runtimeLinker.h"
#undef __APICALL_EXTERN

#include "core/fileManager/fileManager.h"
#include "core/fileManager/types/type_lib.h"
#include "core/kernel/pthread.h"
#include "core/kernel/pthread_intern.h"
#include "core/memory/memory.h"
#include "formats/elf64.h"
#include "logging.h"
#include "memoryLayout.h"
#include "util/moduleLoader.h"
#include "util/plt.h"
#include "util/virtualmemory.h"
#include "utility/progloc.h"
#include "utility/utility.h"

#include <boost/uuid/detail/sha1.hpp>
#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <mutex>
#include <set>
#include <vector>

LOG_DEFINE_MODULE(RuntimeLinker);

namespace {
// clang-format off

using atexit_func_t = SYSV_ABI void (*)();
using entry_func_t  = SYSV_ABI void (*)(EntryParams const* params, atexit_func_t atexit_func);
using module_func_t = SYSV_ABI int (*)(size_t args, const void* argp, atexit_func_t atexit_func);

// clang-format on

struct FrameS {
  FrameS*   next;
  uintptr_t ret_addr;
};

static Program* g_tlsMainProgram = nullptr;

struct RelocateHandlerStack {
  uint64_t stack[3];
};

SYSV_ABI void missingRelocationHandler(RelocateHandlerStack s) {
  LOG_USE_MODULE(RuntimeLinker);

  auto*       stack    = s.stack;
  auto const* payload  = reinterpret_cast<RelocateHandlerPayload*>(stack[-1]);
  auto const  relIndex = stack[0];

  Symbols::SymbolInfo symInfo = {.name = "<unknown function>"};

  if (payload != nullptr) {
    symInfo = payload->format->getSymbolInfo(relIndex);
  }

  LOG_CRIT(L"Missing Symbol| Name:%S Lib:%S(ver:%d) Mod:%S(major:%d minor:%d)", symInfo.name.data(), symInfo.libraryName.data(), symInfo.libraryVersion,
           symInfo.modulName.data(), symInfo.moduleVersionMajor, symInfo.moduleVersionMinor);
}

SYSV_ABI void tlsDynDestruct(void* tlsBlock) {
  if (tlsBlock != nullptr) delete[] (uint8_t*)tlsBlock;
}

void patchCall(uint64_t const srcFunction, uint64_t const dstFunction) {
  int oldMode;
  memory::protect(srcFunction - 1, 20, SceProtRead | SceProtWrite,
                  &oldMode); // Protect Memory

  uint8_t* ptr = (uint8_t*)srcFunction;

  ptr[0] = 0xFF;
  ptr[1] = 0x25;
  ptr[2] = 0x00;
  ptr[3] = 0x00;
  ptr[4] = 0x00;
  ptr[5] = 0x00;

  *((uint64_t*)&ptr[6]) = dstFunction;
  // memset(ptr + 5, 0x90, 4);
  memory::protect(srcFunction - 1, 20, oldMode); // Protect Memory
  flushInstructionCache(srcFunction, 20);
}

std::string encode(const uint8_t* in) {
  std::string out;
  out.resize(12);

  auto pOut = out.data();

  constexpr const char* tab = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-"};

  for (auto n = 8 / 3; n--;) {
    *pOut++ = tab[(in[0] & 0xfc) >> 2];
    *pOut++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
    *pOut++ = tab[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)];
    *pOut++ = tab[in[2] & 0x3f];
    in += 3;
  }

  *pOut++ = tab[(in[0] & 0xfc) >> 2];
  *pOut++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
  *pOut++ = tab[(in[1] & 0x0f) << 2];

  *pOut = '\0';
  return out;
}

std::string name2nid(std::string_view name) {
  boost::uuids::detail::sha1 sha1;
  sha1.process_bytes((unsigned char*)name.data(), name.size());
  sha1.process_bytes("\x51\x8d\x64\xa6\x35\xde\xd8\xc1\xE6\xB0\x39\xB1\xC3\xE5\x52\x30", 16);

  unsigned hash[5];
  sha1.get_digest(hash);

  uint64_t const digest = ((uint64_t)hash[0] << 32u) | hash[1];
  return encode((uint8_t*)&digest);
}
} // namespace

class InternalLib: public Symbols::IResolve {
  std::unordered_map<std::string_view, LibraryId> const m_exportedLibs;

  std::unordered_map<std::string_view, LibraryId> const                        m_importedLibs {}; /// dummy
  std::unordered_map<std::string_view, std::shared_ptr<Symbols::SymbolExport>> m_symExport;       /// [Module name] symbolsExport

  public:
  InternalLib(std::string_view const modulName, std::shared_ptr<Symbols::SymbolExport>& symExport)
      : m_exportedLibs({std::make_pair(symExport->libraryName, LibraryId {})}) {
    m_symExport.insert({modulName, symExport});
  }

  void addModule(std::shared_ptr<Symbols::SymbolExport>& symExport) {
    if (auto it = m_symExport.find(symExport->modulName); it != m_symExport.end()) {
      // add to existing

      if (it->second->symbolsMap.size() > symExport->symbolsMap.size()) {
        it->second->symbolsMap.insert(symExport->symbolsMap.begin(), symExport->symbolsMap.end());
      } else {
        // add to biggest and swap (owns symExport)
        symExport->symbolsMap.insert(it->second->symbolsMap.begin(), it->second->symbolsMap.end());
        it->second->symbolsMap.swap(symExport->symbolsMap);
      }
    } else {
      // inster new
      m_symExport.insert({symExport->modulName, std::move(symExport)});
    }
  }

  std::unordered_map<std::string_view, LibraryId> const& getExportedLibs() const final { return m_exportedLibs; }

  std::unordered_map<std::string_view, LibraryId> const& getImportedLibs() const final { return m_importedLibs; }

  std::string_view const getLibName() const { return m_exportedLibs.begin()->first; }

  uintptr_t getAddress(std::string_view symName, std::string_view libName, std::string_view modName) const final {
    auto it = m_symExport.find(modName);
    if (it == m_symExport.end()) return 0;

    auto itSym = it->second->symbolsMap.find(std::string(symName));
    if (itSym == it->second->symbolsMap.end()) {
      // Special case: check libScePosix aswell
      if (libName == "libkernel" && modName == "libkernel") {
        auto iResolve = accessRuntimeLinker().getIResolve("libScePosix");
        if (iResolve != nullptr) return iResolve->getAddress(symName, "libScePosix", modName);
      }
      // -
      return 0;
    }

    return itSym->second.vaddr;
  }

  uintptr_t getAddress(std::string_view symName) const final {
    for (auto const& mod: m_symExport) {
      auto itSym = mod.second->symbolsMap.find(std::string(symName));
      return itSym->second.vaddr;
    }

    return 0;
  }
};

class RuntimeLinker: public IRuntimeLinker {
  private:
  mutable std::recursive_mutex m_mutex_int;

  uint64_t m_invalidMemoryAddr   = 0;
  size_t   m_countcreatePrograms = 0;

  std::list<std::string>   m_argsSave;
  std::vector<const char*> m_entryParams = {"psOff"};

  /**
   * @brief Programlist (<fixed> 0: mainProgram)
   *
   */
  std::list<std::pair<std::unique_ptr<Program>, std::shared_ptr<IFormat>>> m_programList;
  std::unordered_map<std::string_view, std::shared_ptr<InternalLib>>       m_internalLibs;
  std::unordered_map<std::string_view, std::shared_ptr<Symbols::IResolve>> m_libsMap;
  std::unordered_map<std::string_view, Symbols::SymbolIntercept>           m_interceptMap;
  mutable std::unordered_map<uintptr_t, uintptr_t>                         m_interceptOrigVaddrMap;

  std::unordered_map<uint32_t, Program*>      mTlsIndexMap;
  std::unordered_map<std::string_view, void*> m_libHandles;

  std::vector<uint8_t>                    m_tlsStaticInitBlock;
  std::unordered_map<int, ScePthread_obj> m_threadList;

  uint64_t m_curDTVVersion = 1;

  std::array<DTVKey, DTV_MAX_KEYS> m_dtvKeys;

  bool m_checkOldLib = true; /// Skip check for oldLib in addExport if already found

  void initTlsStaticBlock();
  void setupTlsStaticBlock();

  void loadModules(std::string_view libName);

  public:
  RuntimeLinker() { m_programList.push_back({}); }

  virtual ~RuntimeLinker() = default;

  // Program* := access to unique_ptr
  Program* findProgram(uint64_t vaddr) final;
  Program* findProgramById(size_t id) const final;

  void  callInitProgramms() final;
  void* getTLSAddr(uint32_t index, uint64_t offset) final;

  void     setTLSKey(uint32_t key, const void* value) final;
  void*    getTLSKey(uint32_t key) final;
  uint32_t createTLSKey(void* destructor) final;
  void     deleteTLSKey(uint32_t key) final;
  void     destroyTLSKeys(uint8_t* obj) final;

  std::unique_ptr<Program> createProgram(std::filesystem::path const filepath, uint64_t const baseSize, uint64_t const baseSizeAligned, uint64_t const alocSize,
                                         bool useStaticTLS) final {
    std::unique_lock const lock(m_mutex_int);
    if (useStaticTLS) ++m_countcreatePrograms;
    auto inst = std::make_unique<Program>(filepath, baseSize, baseSizeAligned, IMAGE_BASE, alocSize, useStaticTLS);
    inst->id  = accessFileManager().addFile(createType_lib(inst.get()), filepath);
    return inst;
  }

  uint64_t getAddrInvalidMemory() final {
    std::unique_lock const lock(m_mutex_int);
    return m_invalidMemoryAddr;
  }

  Program* addProgram(std::unique_ptr<Program>&& prog, std::shared_ptr<IFormat> format) final;

  int loadStartModule(std::filesystem::path const& path, size_t args, const void* argp, int* pRes) final;

  uintptr_t relocate() final;

  void stopModules() final;
  void stopModule(int moduleId) final;

  Symbols::IResolve const* getIResolve(std::string_view libName) const final {
    auto const it = m_libsMap.find(libName);
    if (it == m_libsMap.end()) return nullptr;

    return it->second.get();
  }

  uintptr_t checkIntercept(uintptr_t vaddr, std::string_view symName, std::string_view libName, std::string_view modName) const final {
    auto it = m_interceptMap.find(symName);
    if (it != m_interceptMap.end()) {
      m_interceptOrigVaddrMap[it->second.vaddr] = vaddr;
      return it->second.vaddr;
    }
    return vaddr;
  }

  void addExport(std::unique_ptr<Symbols::SymbolExport>&& symbols) final {
    std::unique_lock const lock(m_mutex_int);

    auto it = m_internalLibs.find(symbols->libraryName);

    std::shared_ptr<Symbols::SymbolExport> obj = std::move(symbols);
    if (it == m_internalLibs.end()) {
      auto       inserted = m_internalLibs.insert({obj->libraryName, std::make_shared<InternalLib>(obj->modulName, obj)});
      auto const libName  = inserted.first->first;
      m_libsMap.insert({libName, inserted.first->second});

      // Check if Old Lib Name
      if (m_checkOldLib) {
        constexpr std::string_view oldLib = "libSceGnmDriver";
        constexpr std::string_view newLib = "libSceGraphicsDriver";
        if (libName.compare(newLib) == 0) {
          m_checkOldLib = false;
          m_libsMap.insert({oldLib, std::make_shared<InternalLib>(oldLib, obj)});
        }
      }
      // -
    } else {
      it->second->addModule(obj);
    }
  }

  ModulInfo mainModuleInfo() const final {
    auto prog = accessMainProg();
    return {prog->moduleInfoEx.segments[0].address, prog->moduleInfoEx.segments[0].size, prog->procParamVaddr};
  }

  SceKernelModuleInfoEx const* getModuleInfoEx(uint64_t vaddr) const final {
    std::unique_lock lock(m_mutex_int);

    Program const* prog = nullptr;
    for (auto&& p: m_programList) {
      if (p.second.get()->containsAddr(vaddr, p.first.get()->baseVaddr)) {
        prog = p.first.get();
        break;
      }
    }
    if (prog == nullptr) return nullptr;

    return &prog->moduleInfoEx;
  }

  std::vector<int> getModules() const final {
    std::unique_lock lock(m_mutex_int);
    std::vector<int> ret(m_programList.size());

    Program const* prog = nullptr;

    auto pDst = ret.data();
    for (auto& prog: m_programList) {
      *pDst++ = prog.first->id;
    }
    return ret;
  }

  void* getSymbol(std::string_view symName, std::string_view libName, std::string_view modName) const final {
    auto iresolve = getIResolve(libName);
    if (iresolve == nullptr) return nullptr;
    return (void*)iresolve->getAddress(symName, libName, modName);
  }

  void* getSymbol(int moduleId, std::string_view symName, bool isNid) const final {
    std::unique_lock lock(m_mutex_int);

    // Find Module
    IFormat const* format = nullptr;
    for (auto&& prog: m_programList) {
      if (prog.first->id == moduleId) {
        format = prog.second.get();
        break;
      }
    }
    if (format == nullptr) return nullptr;
    // -

    std::string name;
    if (isNid) {
      name = std::string(symName);
    } else {
      name = name2nid(symName);
      name.pop_back();
    }

    for (auto const& [libName, libId]: format->getExportedLibs()) {
      auto iresolve = getIResolve(libName);
      if (iresolve == nullptr) continue;

      if (auto const addr = (void*)iresolve->getAddress(name); addr != 0) {
        return addr;
      }
    }

    return nullptr;
  }

  void interceptAdd(uintptr_t addr, std::string_view name, std::string_view libraryName, std::string_view modulName) final {
    m_interceptMap[name] = Symbols::SymbolIntercept {.vaddr = addr, .name = name, .libraryName = libraryName, .modulName = modulName};
  }

  uintptr_t interceptGetAddr(uintptr_t addr) const final { return m_interceptOrigVaddrMap[addr]; }

  bool interceptInternal(Program* prog, uintptr_t progaddr, uintptr_t iaddr);

  void cxa_add_atexit(CxaDestructor&& dest, int moduleId) final {
    std::unique_lock const lock(m_mutex_int);
    auto                   prog = findProgramById(moduleId);
    if (prog == nullptr) return;

    prog->cxa.push_back(std::move(dest));
  }

  void cxa_finalize(int moduleId) final {
    std::unique_lock const lock(m_mutex_int);
    auto                   prog = findProgramById(moduleId);
    if (prog == nullptr) return;
    for (auto& c: prog->cxa) {
      c.destructor_func(c.destructor_object);
    }
    prog->cxa.clear();
  }

  void initTLS(uint8_t* tls) final;

  uint64_t getTLSStaticBlockSize() const final { return m_tlsStaticInitBlock.size(); }

  std::vector<std::pair<uint64_t, uint64_t>> getExecSections() const final { return m_programList.begin()->second->getExecSections(); }

  std::vector<const char*> const& getEntryParams() const final { return m_entryParams; };

  void addEntryParam(std::string const& arg) final {
    auto& item = m_argsSave.emplace_back(arg);
    m_entryParams.push_back(item.data());
  }

  Program* accessMainProg() final {
    std::unique_lock const lock(m_mutex_int);
    return m_programList.begin()->first.get();
  }

  Program const* accessMainProg() const {
    std::unique_lock const lock(m_mutex_int);
    return m_programList.begin()->first.get();
  }
};

void RuntimeLinker::initTLS(uint8_t* obj) {
  LOG_USE_MODULE(RuntimeLinker);

  if (!m_tlsStaticInitBlock.empty()) memcpy(&obj[8], m_tlsStaticInitBlock.data(), m_tlsStaticInitBlock.size());
  auto dtvAddress = pthread::getDTV(obj);

  m_threadList[pthread::getThreadId(obj)] = obj;

  dtvAddress[0] = m_curDTVVersion;

  for (auto const& prog: m_programList) {
    if (!prog.first->useStaticTLS) break; // since m_programList is in sequence

    auto& slot = dtvAddress[prog.first->tls.index];
    slot       = (uint64_t)&obj[8 + prog.first->tls.offset];

    LOG_TRACE(L"TLS(%s) slot:%u addr:0x%08llx", prog.first->filename.c_str(), prog.first->tls.index, slot);
  }
}

Program* RuntimeLinker::findProgram(uint64_t vaddr) {
  for (auto&& p: m_programList) {
    if (p.second.get()->containsAddr(vaddr, p.first.get()->baseVaddr)) {
      return p.first.get();
    }
  }
  return nullptr;
}

Program* RuntimeLinker::findProgramById(size_t id) const {
  for (auto&& p: m_programList) {
    if (p.first->id == id) {
      return p.first.get();
    }
  }
  return nullptr;
}

Program* RuntimeLinker::addProgram(std::unique_ptr<Program>&& prog, std::shared_ptr<IFormat> format) {
  LOG_USE_MODULE(RuntimeLinker);
  LOG_INFO(L"adding %s: main:%S id:%llu", prog->filename.c_str(), util::getBoolStr(prog->isMainProg), prog->id);

  prog->relocatePayload.format = format.get();
  prog->relocatePayload.prog   = prog.get();
  format->setupMissingRelocationHandler(prog.get(), reinterpret_cast<void*>(missingRelocationHandler), &prog->relocatePayload);

  std::unique_lock const lock(m_mutex_int);
  for (auto const& item: format->getExportedLibs()) {
    m_libsMap.insert({item.first, format});
  }

  // Place mainprogram at index 0
  if (prog->isMainProg) {
    prog->failGlobalUnresolved = false;
    g_tlsMainProgram           = prog.get();

    m_programList.begin()->first.swap(prog);
    m_programList.begin()->second.swap(format);
    return m_programList.begin()->first.get();
  } else {
    if (util::endsWith(prog->path.parent_path().string(), "_module")) {
      prog->failGlobalUnresolved = false;
    }

    return m_programList.emplace_back(std::make_pair(std::move(prog), std::move(format))).first.get();
  }
}

void RuntimeLinker::loadModules(std::string_view libName) {
  LOG_USE_MODULE(RuntimeLinker);
  auto funcLoad = [this](std::string_view name) {
    LOG_USE_MODULE(RuntimeLinker);
    if (!m_libHandles.contains(name)) {
      LOG_DEBUG(L"Needs library %S", name.data());

      // 1/2 Sepcial case: old->new, build filepath
      auto filepath = std::format(L"{}/modules/", util::getProgramLoc());
      if (name == "libSceGnmDriver") {
        filepath += L"libSceGraphicsDriver";
      } else {
        filepath += std::wstring(name.begin(), name.end());
      }
      filepath += L".dll";
      //- filepath

      if (std::filesystem::exists(filepath) && !m_libHandles.contains(name)) {
        LOG_DEBUG(L"  load library %s", filepath.c_str());
        auto [handle, symbols] = loadModule(name.data(), filepath.c_str(), 1);

        // 2/2 Sepcial case: old->new
        if (name == "libSceGnmDriver") {
          symbols->modulName = name;
        }
        // -special case
        m_libHandles.emplace(std::make_pair(name, handle));
        accessRuntimeLinker().addExport(std::move(symbols));
      }
    }
  };

  funcLoad(libName);
  // todo include dependency into lib
  if (libName == "libSceNpManager") {
    funcLoad("libSceNpManagerForToolkit");
  }
  // ScePosix is sometimes not loaded together with libkernel -> just allways load it
  else if (libName == "libkernel") {
    funcLoad("libScePosix");
  }
}

int RuntimeLinker::loadStartModule(std::filesystem::path const& path, size_t argc, const void* argp, int* pRes) {
  LOG_USE_MODULE(RuntimeLinker);
  std::unique_lock const lock(m_mutex_int);

  // check if already loaded
  auto fileName = path.filename().string();
  if (auto it = std::find_if(m_programList.begin(), m_programList.end(),
                             [&fileName](std::pair<std::unique_ptr<Program>, std::shared_ptr<IFormat>>& rhs) { return rhs.first->filename == fileName; });
      it != m_programList.end()) {
    return it->first->id;
  }
  // -

  auto ifFile = util::openFile(path);
  if (!ifFile) {
    LOG_ERR(L"Couldn't open file %s", path.c_str());
    return getErr(ErrCode::_ENOENT);
  }

  // Load Module
  std::shared_ptr<IFormat> format = buildParser_Elf64(std::filesystem::path(path), std::move(ifFile));
  if (!format->init()) {
    return 0;
  }

  uint64_t baseSize = 0, baseSizeAligned = 0, allocSize = 0;
  format->getAllocInfo(baseSize, baseSizeAligned, allocSize);

  auto program = accessRuntimeLinker().createProgram(format->getFilepath(), baseSize, baseSizeAligned, allocSize, false);

  if (!format->load2Mem(program.get())) {
    LOG_CRIT(L"<--- Parse (%s): Error:accessRuntimeLinker", format->getFilename().c_str());
    return 0;
  }

  auto pProgram = accessRuntimeLinker().addProgram(std::move(program), format);
  // - Module loaded

  pProgram->tls.offset = 0; // Dynamically loaded modules use their own blocks
  auto const tlsIndex  = createTLSKey((void*)tlsDynDestruct);
  pProgram->tls.index  = tlsIndex;

  pProgram->moduleInfoEx.tls_index = tlsIndex;

  mTlsIndexMap[tlsIndex] = pProgram;

  // check imports
  LOG_DEBUG(L"Load for %S", pProgram->filename.string().c_str());
  for (auto const& impLib: format->getImportedLibs()) {
    loadModules(impLib.first);
  }
  // - imports

  // relocate
  auto libName = pProgram->filename.stem().string();
  for (auto& prog: m_programList) {
    if (prog.second->getImportedLibs().find(libName) == prog.second->getImportedLibs().end()) continue;

    prog.second->relocate(prog.first.get(), m_invalidMemoryAddr, libName);
  }
  format->relocate(pProgram, m_invalidMemoryAddr, "");

  uintptr_t const entryAddr = pProgram->entryOffAddr + pProgram->baseVaddr;
  LOG_INFO(L"-> Starting %s entry:0x%08llx tlsIndex:%u", pProgram->filename.c_str(), entryAddr, tlsIndex);
  int result = ((module_func_t)entryAddr)(argc, argp, nullptr);
  if (pRes != nullptr) *pRes = result;
  pProgram->started = true;
  LOG_INFO(L"<- Started %s entry:0x%08llx", pProgram->filename.c_str(), entryAddr);
  // callInitProgramms();

  return pProgram->id;
}

/**
 * @brief Access Static DTV
 * @return void* address
 */
void* tlsMainGetAddr(int32_t offset) {
  LOG_USE_MODULE(RuntimeLinker);
  uint64_t const tlsAddr = (uint64_t)&pthread::getDTV(pthread::getSelf())[0];
  LOG_TRACE(L"[%d] tlsMainGetAddr  addr:0x%08llx offset:0x%0lx", pthread::getThreadId(), tlsAddr, offset);
  return &pthread::getDTV(pthread::getSelf())[0] + offset; // dtv
}

void* tlsMainGetAddr64(int64_t offset) {
  LOG_USE_MODULE(RuntimeLinker);
  uint64_t const tlsAddr = (uint64_t)&pthread::getDTV(pthread::getSelf())[0];
  LOG_TRACE(L"[%d] tlsMainGetAddr64  addr:0x%08llx offset:0x%08llx", pthread::getThreadId(), tlsAddr, offset);
  return &pthread::getDTV(pthread::getSelf())[0] + offset; // dtv
}

void* RuntimeLinker::getTLSAddr(uint32_t index, uint64_t offset) {
  std::unique_lock const lock(m_mutex_int);

  LOG_USE_MODULE(RuntimeLinker);

  if (index <= m_countcreatePrograms) {
    auto& tlsAddr = pthread::getDTV(pthread::getSelf())[index];
    LOG_TRACE(L"[%d] getTlsAddr key:%d addr:0x%08llx offset:0x%08llx", pthread::getThreadId(), index, tlsAddr, offset);
    return (void*)((uint8_t*)tlsAddr + offset);
  }

  // Lookup module and get its tls
  auto const prog    = mTlsIndexMap[index];
  auto&      tlsAddr = pthread::getDTV(pthread::getSelf())[prog->tls.index];

  if (tlsAddr == 0) {
    LOG_TRACE(L"[%d] create tls key:%d", pthread::getThreadId(), index);
    auto obj = std::make_unique<uint8_t[]>(prog->tls.sizeImage).release();
    memcpy(obj, (void*)prog->tls.vaddrImage, prog->tls.sizeImage);
    tlsAddr = (uint64_t)obj;
  }

  LOG_TRACE(L"[%d] getTlsAddr key:%d addr:0x%08llx offset:0x%08llx", pthread::getThreadId(), index, tlsAddr, offset);
  return (void*)((uint8_t*)tlsAddr + offset);
}

void RuntimeLinker::setTLSKey(uint32_t index, const void* value) {
  LOG_USE_MODULE(RuntimeLinker);

  std::unique_lock const lock(m_mutex_int);

  auto dtv     = pthread::getDTV(pthread::getSelf());
  auto pDtvKey = &dtv[index];

  LOG_TRACE(L"[thread:%d] TLS Key| -> key:%d value:0x%08llx", pthread::getThreadId(), index, (uint64_t)value);
  *pDtvKey = (uint64_t)value;
}

void* RuntimeLinker::getTLSKey(uint32_t index) {
  LOG_USE_MODULE(RuntimeLinker);

  std::unique_lock const lock(m_mutex_int);

  auto dtv     = pthread::getDTV(pthread::getSelf());
  auto pDtvKey = &dtv[index];

  LOG_TRACE(L"[thread:%d] TLS Key| <- key:%d value:0x%08llx", pthread::getThreadId(), index, pDtvKey[index]);
  return (void*)*pDtvKey;
}

uint32_t RuntimeLinker::createTLSKey(void* destructor) {
  LOG_USE_MODULE(RuntimeLinker);

  std::unique_lock const lock(m_mutex_int);

  for (uint64_t n = m_countcreatePrograms; n < m_dtvKeys.size(); ++n) {
    if (!m_dtvKeys[n].used) {
      m_dtvKeys[n].used       = true;
      m_dtvKeys[n].destructor = (pthread_key_destructor_func_t)destructor;
      LOG_TRACE(L"-> TLS Key:%d destructor:0x%08llx", n, reinterpret_cast<uint64_t>(destructor));
      return n;
    }
  }
  LOG_ERR(L"Not enough dtv space");
  return -1;
}

void RuntimeLinker::deleteTLSKey(uint32_t key) {
  LOG_USE_MODULE(RuntimeLinker);

  std::unique_lock const lock(m_mutex_int);

  auto destructor = m_dtvKeys[key].destructor;
  for (auto obj: m_threadList) {
    auto dtv = pthread::getDTV(obj.second);
    dtv[key] = 0;
  }

  m_dtvKeys[key].used       = false;
  m_dtvKeys[key].destructor = nullptr;

  LOG_DEBUG(L"<- TLS Key:%d thread:%d", key, pthread::getThreadId());
}

void RuntimeLinker::destroyTLSKeys(uint8_t* obj) {
  auto pDtvKey = pthread::getDTV(obj);

  std::unique_lock const lock(m_mutex_int);

  for (uint64_t n = m_countcreatePrograms; n < m_dtvKeys.size(); ++n, ++pDtvKey) {
    if (m_dtvKeys[n].destructor != nullptr) {
      if (pDtvKey[n] != 0) m_dtvKeys[n].destructor((void*)pDtvKey[n]);
    }
  }

  m_threadList.erase(pthread::getThreadId(obj));
}

void RuntimeLinker::stopModules() {
  LOG_USE_MODULE(RuntimeLinker);

  // Stop Modules
  for (auto& prog: m_programList) {
    LOG_INFO(L"Stopping module %s", prog.first->filename.c_str());
    // prog.second->dtDeinit(prog.first->baseVaddr);
    for (auto& c: prog.first->cxa) {
      c.destructor_func(c.destructor_object);
    }
    prog.first->cxa.clear();
  }

  // // Custom Libs
  // for (auto const& item: m_libHandles) {
  //   unloadModule(item.second);
  // }
}

void RuntimeLinker::stopModule(int moduleId) {
  LOG_USE_MODULE(RuntimeLinker);

  for (auto itProg = m_programList.begin(); itProg != m_programList.end(); ++itProg) {
    if (itProg->first->id == moduleId) {
      LOG_INFO(L"Stopping module %s", itProg->first->filename.c_str());
      for (auto& c: itProg->first->cxa) {
        c.destructor_func(c.destructor_object);
      }
      itProg->first->cxa.clear();

      m_programList.erase(itProg);
      break;
    }
  }
}

void RuntimeLinker::callInitProgramms() {
  LOG_USE_MODULE(RuntimeLinker);

  struct CallGraph {
    Program* const         program;
    IFormat* const         iFormat;
    std::vector<CallGraph> childs;

    CallGraph(Program* program, IFormat* format): program(program), iFormat(format) {}
  };

  auto      itMainProg = m_programList.begin();
  CallGraph callGraph({itMainProg->first.get(), itMainProg->second.get()});
  // Get dependencies
  for (auto const& impLib: itMainProg->second->getImportedLibs()) {
    for (auto itImp = std::next(m_programList.begin()); itImp != m_programList.end(); ++itImp) {
      auto const& expLib = itImp->second->getExportedLibs();
      if (expLib.find(impLib.first) != expLib.end()) {
        LOG_DEBUG(L"%s needs %S", itMainProg->first->filename.c_str(), impLib.first.data());
        callGraph.childs.push_back({itImp->first.get(), itImp->second.get()});
        break;
      }
    }
  }

  for (auto& parent: callGraph.childs) {
    auto const& impParent = parent.iFormat->getImportedLibs();
    for (auto itImp = std::next(m_programList.begin()); itImp != m_programList.end(); ++itImp) {
      for (auto const& expLib: itImp->second->getExportedLibs()) {
        if (impParent.find(expLib.first) != impParent.end()) {
          LOG_DEBUG(L"%s needs %s", parent.program->filename.c_str(), itImp->first->filename.c_str());
          parent.childs.push_back({itImp->first.get(), itImp->second.get()});
        }
      }
    }
  }
  //- dependencies

  // Startup modules
  auto const startModule = [=](CallGraph const& item) {
    if (item.program->started) return;
    item.program->started = true;

    LOG_INFO(L"Starting %s", item.program->filename.c_str());
    item.iFormat->dtInit(item.program->baseVaddr);
  };

  for (auto& parent: callGraph.childs) {
    for (auto& child: parent.childs) {
      startModule(child);
    }
    startModule(parent);
  }
}

void RuntimeLinker::initTlsStaticBlock() {
  LOG_USE_MODULE(RuntimeLinker);

  // Set TLS Static Block
  size_t tlsStaticSize = 0;
  {
    int64_t offsetPre = 0;
    for (auto const& prog: m_programList) {
      offsetPre = util::alignDown(offsetPre - prog.first->tls.sizeImage, prog.first->tls.alignment);
    }
    tlsStaticSize = util::alignUp(std::abs(offsetPre), 32);
  }

  m_tlsStaticInitBlock.resize(tlsStaticSize);
  int64_t  offset     = tlsStaticSize;
  uint32_t indexCount = 0;
  for (auto const& prog: m_programList) {
    offset = util::alignDown(offset - prog.first->tls.sizeImage, prog.first->tls.alignment);

    prog.first->moduleInfoEx.tls_index  = ++indexCount; // has to start with 1
    prog.first->moduleInfoEx.tls_offset = offset;

    prog.first->tls.offset = offset;
    prog.first->tls.index  = prog.first->moduleInfoEx.tls_index;

    m_dtvKeys[prog.first->tls.index].used       = true;
    m_dtvKeys[prog.first->tls.index].destructor = nullptr;

    LOG_DEBUG(L"%s| tls index:%u offset:0x%08llx alignment:%d", prog.first->filename.c_str(), prog.first->tls.index, prog.first->tls.offset,
              prog.first->tls.alignment);
  }
  // -

  assert((int64_t)offset >= 0);
}

void RuntimeLinker::setupTlsStaticBlock() {
  for (auto const& prog: m_programList) {
    memcpy(&m_tlsStaticInitBlock[prog.first->tls.offset], (void*)prog.first->tls.vaddrImage, prog.first->tls.sizeImage);
  }
}

bool RuntimeLinker::interceptInternal(Program* prog, uintptr_t progoffset, uintptr_t iaddr) {
#pragma pack(push, 1)

  struct jumper {
    const uint16_t movrax = 0xb848;
    uint64_t       addr;
    const uint16_t jmprax = 0xe0ff;
  };

#pragma pack(pop)

  const auto progaddr = prog->baseVaddr + progoffset;

  int iProts[2] = {0, 0};

  const jumper j = {.addr = iaddr};

  if (!memory::protect(progaddr, sizeof(jumper), 7 /* Set exec, read and write prot to the program's code */, &iProts[0])) return false;
  ::memcpy((void*)progaddr, (const void*)&j, sizeof(jumper)); // Copy the jumper to the prgoram
  if (!memory::protect(progaddr, sizeof(jumper), iProts[0] /* Restore the old prot */, &iProts[1])) return false;

  return true;
}

uintptr_t RuntimeLinker::relocate() {
  LOG_USE_MODULE(RuntimeLinker);
  LOG_INFO(L"Relocate()");

  initTlsStaticBlock();
  // pthread::initSelfForMainThread(m_tlsStaticInitBlock.size()); // call before runtimelinker
  // initTLS(pthread::getSelf());

  // Get and load additional Modules needed
  {
    for (auto const& prog: m_programList) {
      LOG_DEBUG(L"Load for %s", prog.first->filename.c_str());
      for (auto const& impLib: prog.second->getImportedLibs()) {
        loadModules(impLib.first);
      }
    }
  }
  // - load Modules

  // Relocate all (Set Imported Symbols)
  m_invalidMemoryAddr = memory::alloc(INVALID_MEMORY, 4096, 0);
  for (auto& prog: m_programList) {
    if (prog.first) prog.second->relocate(prog.first.get(), m_invalidMemoryAddr, "");
  }

  setupTlsStaticBlock(); // relocate may init tls -> copy after relocate

  uintptr_t const entryAddr = m_programList.begin()->first->entryOffAddr + m_programList.begin()->first->baseVaddr;
  LOG_INFO(L"entry:0x%08llx", entryAddr);

  return entryAddr;
}

IRuntimeLinker& accessRuntimeLinker() {
  static RuntimeLinker inst;
  return inst;
}
