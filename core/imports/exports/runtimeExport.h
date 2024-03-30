#pragma once

#include "utility/utility.h"

#include <filesystem>
#include <stdint.h>

using cxa_destructor_func_t = void (*)(void*);

struct CxaDestructor {
  cxa_destructor_func_t destructor_func;
  void*                 destructor_object;
};

#pragma pack(1)

struct alignas(32) EntryParams {
  int         argc    = 0;
  uint32_t    pad     = 0;
  const char* argv[3] = {0, 0, 0};
};

struct ModulInfo {
  uint64_t address;
  uint64_t size;
  uint64_t procParamAddr;
};

struct SceKernelModuleSegmentInfo {
  uint64_t address;
  uint32_t size;
  int      prot;
};

struct SceKernelModuleInfo {
  uint64_t                   size = sizeof(SceKernelModuleInfo);
  char                       name[255];
  SceKernelModuleSegmentInfo segments[3];
  uint32_t                   segment_count;
  uint32_t                   ref_count;
  uint8_t                    fingerprint[20];
};

struct SceKernelModuleInfoEx {
  uint64_t size = sizeof(SceKernelModuleInfoEx);
  char     name[255];
  int      id;
  uint32_t tls_index;
  void*    tls_init_addr;
  uint32_t tls_init_size;
  uint32_t tls_size;
  uint32_t tls_offset;
  uint32_t tls_align;
  uint64_t init_proc_addr;
  uint64_t fini_proc_addr;
  uint64_t reserved1 = 0;
  uint64_t reserved2 = 0;
  void*    eh_frame_hdr_addr;
  void*    eh_frame_addr;
  uint32_t eh_frame_hdr_size;
  uint32_t eh_frame_size;

  SceKernelModuleSegmentInfo segments[3];
  uint32_t                   segment_count;
  uint32_t                   ref_count;
};

#pragma pack()

class IRuntimeExport {
  CLASS_NO_COPY(IRuntimeExport);
  CLASS_NO_MOVE(IRuntimeExport);

  protected:
  IRuntimeExport() = default;

  public:
  virtual ~IRuntimeExport() = default;

  virtual int loadStartModule(std::filesystem::path const& path, size_t args, const void* argp, int* pRes) = 0;

  virtual void initTLS(uint8_t* obj) = 0;

  virtual uint64_t getTLSStaticBlockSize() const = 0;

  virtual EntryParams const* getEntryParams() const = 0;

  virtual void cxa_add_atexit(CxaDestructor&&, int moduleId) = 0;
  virtual void cxa_finalize(int moduleId)                    = 0;

  virtual ModulInfo mainModuleInfo() const = 0;

  virtual SceKernelModuleInfoEx const* getModuleInfoEx(uint64_t vaddr) const = 0;

  virtual std::vector<int> getModules() const = 0;

  /**
   * @brief Get the symbols address
   *
   * @param symName nid
   * @param libName
   * @param modName
   * @return void* address
   */
  virtual void* getSymbol(std::string_view symName, std::string_view libName, std::string_view modName) const = 0;

  /**
   * @brief Searches the module for the symbol
   *
   * @param module moduleId
   * @param symName
   * @param isNid
   * @return void*
   */
  virtual void* getSymbol(int module, std::string_view symName, bool isNid) const = 0;

  // ### THREAD LOCAL STORAGE
  virtual void*    getTLSAddr(uint32_t index, uint64_t offset) = 0;
  virtual void     setTLSKey(uint32_t key, const void* value)  = 0;
  virtual void*    getTLSKey(uint32_t key)                     = 0;
  virtual uint32_t createTLSKey(void* destructor)              = 0;
  virtual void     deleteTLSKey(uint32_t key)                  = 0;
  virtual void     destroyTLSKeys(uint8_t* obj)                = 0;
  // - ### TLS
};
