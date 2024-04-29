#include "exceptionHandler.h"

#include "logging.h"

#include <magic_enum/magic_enum.hpp>
#include <mutex>
#include <windows.h>

LOG_DEFINE_MODULE(ExceptionHandler);

namespace {
enum class AccessViolationType { Unknown, Read, Write, Execute };

LONG WINAPI DefaultExceptionHandler(PEXCEPTION_POINTERS exception) {
  LOG_USE_MODULE(ExceptionHandler);

  PEXCEPTION_RECORD exceptionRecord = exception->ExceptionRecord;

  auto exceptionAddr = (uint64_t)(exceptionRecord->ExceptionAddress);
  auto violationAddr = exceptionRecord->ExceptionInformation[1];

  AccessViolationType violationType;
  switch (exceptionRecord->ExceptionInformation[0]) {
    case 0: violationType = AccessViolationType::Read; break;
    case 1: violationType = AccessViolationType::Write; break;
    case 8: violationType = AccessViolationType::Execute; break;
    default: LOG_CRIT(L"unknown exception at 0x%08llx", exceptionAddr); break;
  }

  LOG_CRIT(L"Access violation: %S at addr:0x%08llx info:0x%08llx", magic_enum::enum_name(violationType).data(), exceptionAddr, violationAddr);
  //(violationVaddr == accessRuntimeLinker().getAddrInvalidMemory() ? L"(Unpatched object)" : L""));

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