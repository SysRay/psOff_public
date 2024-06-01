#define __APICALL_EXTERN
#include "logging.h"
#undef __APICALL_EXTERN

#include "config_emu.h"

#define P7TRACE_NO_VA_ARG_OPTIMIZATION
#include <cassert>
#include <chrono>
#include <mutex>
#include <unordered_map>

// clang-format off

#include <p7/GTypes.h>
#include <p7/P7_Telemetry.h>
#include <p7/P7_Trace.h>
#include <p7/P7_Extensions.h>

// clang-format on

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

const wchar_t* getParams(std::wstring& params) {
  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::LOGGING);

  auto readParam = [&params](json* field, json::value_t jsontype, const wchar_t* p7param) -> bool {
    try {
      std::string temp;
      int         tempi;

      switch (jsontype) {
        case json::value_t::string: field->get_to(temp); break;

        case json::value_t::number_integer: temp = std::to_string(field->get_to(tempi)); break;

        default: printf("Missing type handler for json_t(%d)!\n", (int)jsontype); return false;
      }

      params.append(L" ");
      params.append(p7param);
      params.append(std::wstring(temp.begin(), temp.end()));
      return true;
    } catch (const json::exception& e) {
      printf("Logger parameter parse fail: %s\n", e.what());
    }

    return false;
  };

  readParam(&(*jData)["sink"], json::value_t::string, L"/P7.Sink=");
  readParam(&(*jData)["verbosity"], json::value_t::number_integer, L"/P7.Trc.Verb=");

  return params.c_str();
}

void* __registerLoggingModule(std::wstring_view name) {
  auto trace = getTrace();
  {
    const std::unique_lock lock(getMutex());
    if (*getClient() == nullptr) {
      /*
        Default parameter, the rest will be appended.
        P7.Files indicates how much CLIENT logs P7
        can generate. Should be 0, the only log we
        interested in is the trace.
      */
      std::wstring params = L"/P7.Pool=1024 /P7.Files=0 /P7.Roll=5hr";
      *getClient()        = P7_Create_Client(getParams(params));
      *trace              = P7_Create_Trace(*getClient(), __APPNAME);
    }
  }

  auto getCustomVerb = [](std::wstring_view name) -> int32_t {
    int32_t     vlevel;
    std::string sname(name.begin(), name.end());
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::LOGGING);

    auto& modver = (*jData)["_customVerb"][sname];
    if (modver.is_number_integer()) {
      printf("Custom verbosity set for: %ls\n", name.data());
      return modver.get_to(vlevel);
    }

    return -1;
  };

  IP7_Trace::hModule pModule;
  (*trace)->Register_Module(name.data(), &pModule);
  auto lvl = getCustomVerb(name);
  if (lvl != -1) (*trace)->Set_Verbosity(pModule, (eP7Trace_Level)lvl);
  return pModule;
}

void flush() {
  P7_Client_Flush(*getClient());
}

void flushExceptional() {
  P7_Exceptional_Flush();
}

uint8_t isIgnored(void* module, eTrace_Level level) {
  return static_cast<uint8_t>((*getTrace())->Get_Verbosity(module)) <= static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(level);
}

void __log(eTrace_Level level, void* hmodule, unsigned short i_wLine, const char* i_pFile, const char* i_pFunction, const wchar_t* i_pFormat, ...) {
  // EXIT_IF(mTrace == nullptr);
  va_list args = nullptr;
  va_start(args, i_pFormat);

  if (static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(level) ==
      static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(eTrace_Level::err)) {
    printf("%s| Error:", ((sP7Trace_Module*)hmodule)->pName);
    vwprintf(i_pFormat, args);
    printf("\n");
  } else if (static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(level) ==
             static_cast<typename std::underlying_type<__Log::eTrace_Level>::type>(eTrace_Level::crit)) {
    printf("%s| Critical Error:", ((sP7Trace_Module*)hmodule)->pName);
    vwprintf(i_pFormat, args);
    printf("\nExiting\n");
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
