#include "exceptionHandler.h"

#include "../runtimeLinker.h"
#include "logging.h"

#include <boost/stacktrace.hpp>
#include <magic_enum/magic_enum.hpp>
#include <mutex>

// clang-format off
#include <windows.h>
#include <psapi.h>
// clang-format on

LOG_DEFINE_MODULE(ExceptionHandler);

namespace {

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
    int         lineNumber   = 0;
    DWORD       displacement = 0;

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
        LOG_ERR(L"0x%08llx\t %S at %llu base:0x%08llx %S", trace.address(), fileName.c_str(), lineNumber, optModuleInfo->first, optModuleInfo->second.c_str());
      } else {
        LOG_ERR(L"0x%08llx\t %S at %llu", trace.address(), fileName.c_str(), lineNumber);
      }
    }
  }
  // -
}

enum class AccessViolationType { Unknown, Read, Write, Execute };

LONG WINAPI DefaultExceptionHandler(PEXCEPTION_POINTERS exception) {
  LOG_USE_MODULE(ExceptionHandler);

  PEXCEPTION_RECORD exceptionRecord = exception->ExceptionRecord;

  auto exceptionAddr = (uint64_t)(exceptionRecord->ExceptionAddress);
  auto violationAddr = exceptionRecord->ExceptionInformation[1];

  stackTrace(exceptionAddr);

  uint64_t    baseAddr = 0;
  std::string moduleName;

  auto optModuleInfo = findModule(exceptionAddr);
  if (optModuleInfo) {
    baseAddr   = optModuleInfo->first;
    moduleName = optModuleInfo->second;
  }

  AccessViolationType violationType;
  switch (exceptionRecord->ExceptionInformation[0]) {
    case 0: violationType = AccessViolationType::Read; break;
    case 1: violationType = AccessViolationType::Write; break;
    case 8: violationType = AccessViolationType::Execute; break;
    default: LOG_CRIT(L"unknown exception at 0x%08llx, module base:0x%08llx %S", exceptionAddr, baseAddr, moduleName.data()); break;
  }

  LOG_CRIT(L"Access violation: %S at addr:0x%08llx info:0x%08llx %S, module base:0x%08llx %S", magic_enum::enum_name(violationType).data(), exceptionAddr,
           violationAddr, (violationAddr == accessRuntimeLinker().getAddrInvalidMemory() ? L"(Unpatched object)" : L""), baseAddr, moduleName.data());
  //);

  return EXCEPTION_CONTINUE_EXECUTION;
}

std::once_flag INSTALL_FLAG;
} // namespace

namespace ExceptionHandler {

std::unique_ptr<uint8_t[]> install(uint64_t imageAddr, uint64_t imageSize) {
  auto funcTableData = std::make_unique<uint8_t[]>(sizeof(RUNTIME_FUNCTION));

  auto functionTable = (RUNTIME_FUNCTION*)funcTableData.get();

  functionTable->BeginAddress = 0;
  functionTable->EndAddress   = imageSize;
  functionTable->UnwindData   = 0;

  RtlAddFunctionTable(functionTable, 1, imageAddr);

  std::call_once(INSTALL_FLAG, [] { AddVectoredExceptionHandler(1, DefaultExceptionHandler); });

  return funcTableData;
}
} // namespace ExceptionHandler