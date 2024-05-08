#include "exceptionHandler.h"

#include "../runtimeLinker.h"
#include "core/memory/memory.h"
#include "core/runtime/util/virtualmemory.h"
#include "logging.h"
#include "utility/progloc.h"

#include <boost/scoped_ptr.hpp>
#include <boost/stacktrace.hpp>
#include <magic_enum/magic_enum.hpp>
#include <mutex>
// clang-format off
#include <windows.h>
#include <psapi.h>
#include <DbgHelp.h>
// clang-format on

LOG_DEFINE_MODULE(ExceptionHandler);

namespace {

struct UnwindInfo {
  uint8_t Version : 3;
  uint8_t Flags   : 5;
  uint8_t SizeOfProlog;
  uint8_t CountOfCodes;
  uint8_t FrameRegister : 4;
  uint8_t FrameOffset   : 4;
  ULONG   ExceptionHandler;
  void*   ExceptionData;
};

struct JmpHandlerData {
  void setHandler(uint64_t func) { *(uint64_t*)(&m_code[2]) = func; }

  // mov rax, 0x1122334455667788
  // jmp rax
  uint8_t m_code[16] = {0x48, 0xB8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xFF, 0xE0};
};

struct ExData {
  JmpHandlerData   handlerCode;
  RUNTIME_FUNCTION runtimeFunction;
  UnwindInfo       unwindInfo;
};

std::optional<std::pair<uint64_t, std::string>> findModule(uint64_t address) {

  std::unordered_map<uint64_t, std::string> modules;

  HANDLE  hProcess = GetCurrentProcess();
  HMODULE hModules[1024];
  DWORD   cbNeeded;

  if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
    MODULEINFO mi;

    for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
      GetModuleInformation(hProcess, hModules[i], &mi, sizeof(mi));

      if ((uint64_t)mi.lpBaseOfDll <= address && address < ((uint64_t)mi.lpBaseOfDll + mi.SizeOfImage)) {
        char moduleName[MAX_PATH];
        if (GetModuleFileName(hModules[i], moduleName, MAX_PATH)) {
          return std::make_pair((uint64_t)mi.lpBaseOfDll, std::string(moduleName));
        }
      }
    }
  }

  auto prog = accessRuntimeLinker().findProgram(address);
  if (prog != nullptr) {
    return std::make_pair(prog->baseVaddr, prog->filename.string());
  }

  return std::nullopt;
}

bool tryGetSymName(const void* addr, std::string& name) {
  auto proc = GetCurrentProcess();

  static std::once_flag init;
  std::call_once(init, [proc]() {
    LOG_USE_MODULE(ExceptionHandler);
    auto dir = std::filesystem::path(util::getProgramLoc()) / "debug";
    if (!SymInitializeW(proc, dir.c_str(), true)) {
      LOG_ERR(L"Failed to initialize the debug information");
    }
  });

  boost::scoped_ptr symp(new char[sizeof(SYMBOL_INFO) + 1024]);

  PSYMBOL_INFO sym  = (PSYMBOL_INFO)symp.get();
  sym->SizeOfStruct = sizeof(SYMBOL_INFO);
  sym->MaxNameLen   = 1024;

  DWORD disp = 0;

  IMAGEHLP_LINE64 line = {
      .SizeOfStruct = sizeof(IMAGEHLP_LINE64),
  };

  if (SymFromAddr(proc, (uint64_t)addr, nullptr, sym)) {
    name.assign(sym->Name);
    if (SymGetLineFromAddr64(proc, sym->Address, &disp, &line)) {
      name += std::format(" ({}:{}:{})", line.FileName, line.LineNumber, disp);
    }

    return true;
  }

  return false;
}

void stackTrace(uint64_t addr) {
  LOG_USE_MODULE(ExceptionHandler);

  bool   foundStart  = false;
  size_t countTraces = 0;

  // Stack trace
  for (auto& trace: boost::stacktrace::basic_stacktrace()) {
    if (!foundStart) {

      if ((uint64_t)trace.address() == addr)
        foundStart = true;
      else
        continue;
    }
    if (++countTraces > 4) break;

    if (trace.empty()) {
      LOG_ERR(L"????");
    }

    std::string fileName;

    if (!tryGetSymName(trace.address(), fileName)) {
      // Failed to get the source file name, clearing the string
      fileName.clear();
    }

    auto optModuleInfo = findModule((uint64_t)trace.address());

    if (fileName.empty()) {
      auto optModuleInfo = findModule((uint64_t)trace.address());

      if (optModuleInfo) {
        LOG_ERR(L"offset:0x%08llx\t base:0x%08llx\t%S", (uint64_t)trace.address() - optModuleInfo->first, optModuleInfo->first, optModuleInfo->second.c_str());
      } else {
        LOG_ERR(L"0x%08llx\t", trace.address());
      }
    } else {
      if (optModuleInfo) {
        LOG_ERR(L"0x%08llx base:0x%08llx %S\n\t%S", trace.address(), optModuleInfo->first, optModuleInfo->second.c_str(), fileName.c_str());
      } else {
        LOG_ERR(L"0x%08llx\n\t %S", trace.address(), fileName.c_str());
      }
    }
  }
  // -
}

enum class AccessViolationType { Unknown, Read, Write, Execute };

static EXCEPTION_DISPOSITION DefaultExceptionHandler(PEXCEPTION_RECORD   exception_record, ULONG64 /*EstablisherFrame*/, PCONTEXT /*ContextRecord*/,
                                                     PDISPATCHER_CONTEXT dispatcher_context) {
  LOG_USE_MODULE(ExceptionHandler);

  auto exceptionAddr = (uint64_t)(exception_record->ExceptionAddress);
  auto violationAddr = exception_record->ExceptionInformation[1];

  stackTrace(exceptionAddr);

  uint64_t    baseAddr = 0;
  std::string moduleName;

  auto optModuleInfo = findModule(exceptionAddr);
  if (optModuleInfo) {
    baseAddr   = optModuleInfo->first;
    moduleName = optModuleInfo->second;
  }

  AccessViolationType violationType;
  switch (exception_record->ExceptionInformation[0]) {
    case 0: violationType = AccessViolationType::Read; break;
    case 1: violationType = AccessViolationType::Write; break;
    case 8: violationType = AccessViolationType::Execute; break;
    default: LOG_CRIT(L"unknown exception at 0x%08llx, module base:0x%08llx %S", exceptionAddr, baseAddr, moduleName.data()); break;
  }

  LOG_CRIT(L"Access violation: %S at addr:0x%08llx info:0x%08llx %S, module base:0x%08llx %S", magic_enum::enum_name(violationType).data(), exceptionAddr,
           violationAddr, (violationAddr == accessRuntimeLinker().getAddrInvalidMemory() ? L"(Unpatched object)" : L""), baseAddr, moduleName.data());
  //);

  return ExceptionContinueExecution;
}

} // namespace

namespace ExceptionHandler {
uint64_t getAllocSize() {
  return sizeof(ExData);
}

void install(uint64_t imageAddr, uint64_t handlerDstAddr, uint64_t imageSize) {
  auto functionTable = new ((void*)handlerDstAddr) ExData;

  auto& func        = functionTable->runtimeFunction;
  func.BeginAddress = 0;
  func.EndAddress   = imageSize;
  func.UnwindData   = (uint64_t)&functionTable->unwindInfo - imageAddr;

  auto& unwindData            = functionTable->unwindInfo;
  unwindData.Version          = 1;
  unwindData.Flags            = UNW_FLAG_EHANDLER;
  unwindData.SizeOfProlog     = 0;
  unwindData.CountOfCodes     = 0;
  unwindData.FrameRegister    = 0;
  unwindData.FrameOffset      = 0;
  unwindData.ExceptionHandler = (uint64_t)&functionTable->handlerCode - imageAddr;
  unwindData.ExceptionData    = nullptr;

  functionTable->handlerCode.setHandler((uint64_t)DefaultExceptionHandler);

  RtlAddFunctionTable(&functionTable->runtimeFunction, 1, imageAddr);

  flushInstructionCache((uint64_t)&functionTable->handlerCode, sizeof(JmpHandlerData));
}
} // namespace ExceptionHandler
