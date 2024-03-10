#define __APICALL_EXTERN
#include "logging.h"
#undef __APICALL_EXTERN

#include <P7_Telemetry.h>
#include <P7_Trace.h>
#include <cassert>
#include <chrono>
#include <mutex>
#include <unordered_map>

namespace __Log {

IP7_Client** getClient() {
  static IP7_Client* mClient = nullptr;
  return &mClient;
}

IP7_Trace** getTrace() {
  static IP7_Trace* mTrace = nullptr;
  return &mTrace;
}

std::unordered_map<std::wstring, int>& getThreads() {
  static std::unordered_map<std::wstring, int> mThreads;
  return mThreads;
}

std::mutex& getMutex() {
  static std::mutex mMutex;
  return mMutex;
}

void* __registerLoggingModule(std::wstring&& name) {
  auto trace = getTrace();
  {
    const std::unique_lock lock(getMutex());
    if (*getClient() == nullptr) {
      *getClient() = P7_Create_Client(TM("/P7.Pool=1024"));
      *trace       = P7_Create_Trace(*getClient(), __APPNAME);
    }
  }
  IP7_Trace::hModule pModule;
  (*trace)->Register_Module(name.data(), &pModule);
  return pModule;
}

void deinit() {
  P7_Client_Flush(*getClient());
}

uint8_t isIgnored(void* module, eTrace_Level level) {
  return static_cast<uint8_t>((*getTrace())->Get_Verbosity(module)) <= static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(level);
}

void __log(eTrace_Level level, void* hmodule, unsigned short i_wLine, const char* i_pFile, const char* i_pFunction, const wchar_t* i_pFormat, ...) {
  // EXIT_IF(mTrace == nullptr);
  va_list args = nullptr;
  va_start(args, i_pFormat);

  if (static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(level) >=
      static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(eTrace_Level::err)) {
    vwprintf(i_pFormat, args);
    printf("\n");
  }

  (*getTrace())
      ->Trace_Embedded(0, static_cast<eP7Trace_Level>(static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(level)), hmodule, i_wLine, i_pFile,
                       i_pFunction, &i_pFormat, &args);
  va_end(args);

  if (level == eTrace_Level::crit) {
    P7_Exceptional_Flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // while (!::IsDebuggerPresent())
    //::Sleep(100); // to avoid 100% CPU load
    exit(1);
  }
}
} // namespace __Log