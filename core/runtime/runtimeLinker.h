#pragma once
#include "formats/IFormat.h"
#include "formats/ISymbols.h"
#include "program.h"
#include "runtimeExport.h"

#include <filesystem>
#include <memory>

class IRuntimeLinker: public IRuntimeExport {
  public:
  virtual std::unique_ptr<Program> createProgram(std::filesystem::path const filename, uint64_t const baseSize, uint64_t const baseSizeAligned,
                                                 uint64_t const alocSize, bool useStaticTLS) = 0;

  virtual uint64_t getAddrInvalidMemory() = 0;

  virtual void callInitProgramms() = 0;

  virtual Program* addProgram(std::unique_ptr<Program>&& prog, std::shared_ptr<IFormat> format) = 0;

  virtual Program* findProgram(uint64_t vaddr)      = 0;
  virtual Program* findProgramById(size_t id) const = 0;

  virtual uintptr_t execute()     = 0;
  virtual void      stopModules() = 0;

  virtual void stopModule(int id) = 0;

  virtual void addExport(std::unique_ptr<Symbols::SymbolExport>&& symbols) = 0;

  virtual void      interceptAdd(uintptr_t addr, std::string_view name, std::string_view libraryName, std::string_view modulName) = 0;
  virtual uintptr_t interceptGetAddr(uintptr_t addr) const                                                                        = 0;

  virtual Symbols::IResolve const* getIResolve(std::string_view libName) const = 0;

  virtual uintptr_t checkIntercept(uintptr_t vaddr, std::string_view symName, std::string_view libName, std::string_view modName) const = 0;

  virtual std::vector<std::pair<uint64_t, uint64_t>> getExecSections() const = 0;

  /**
   * @brief get access to the main programm. (unique_ptr)
   *
   * @return Program* nullptr if no main program is missing
   */
  virtual Program* accessMainProg() = 0;

  virtual ~IRuntimeLinker() = default;
};

SYSV_ABI void* tlsMainGetAddr(int32_t offset);
SYSV_ABI void* tlsMainGetAddr64(int64_t offset);

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL IRuntimeLinker& accessRuntimeLinker();

__APICALL std::unique_ptr<IFormat> buildParser_Elf64(std::filesystem::path&& path, std::unique_ptr<std::ifstream>&& file);
#undef __APICALL