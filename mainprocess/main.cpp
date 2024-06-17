#include "eventsystem\events\system_circle\events.h"
#include "eventsystem\events\system_circle\handler.h"
#include "eventsystem\events\system_cross\events.h"
#include "eventsystem\events\system_cross\handler.h"
#include "utility/progloc.h"

#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <thread>
#include <windows.h>

class EmuProcess {
  STARTUPINFOW m_startup = {
      .cb = sizeof(STARTUPINFOA),
  };

  PROCESS_INFORMATION m_procInfo = {
      .hProcess    = INVALID_HANDLE_VALUE,
      .hThread     = INVALID_HANDLE_VALUE,
      .dwProcessId = 0,
      .dwThreadId  = 0,
  };

  std::vector<const char*> m_blacklist = {"--file", "--update", "--root"};
  std::filesystem::path    m_childExec;
  std::wstring             m_args;
  std::wstring             m_currCmdLine;

  std::recursive_mutex m_accessProcInfo;

  public:
  EmuProcess(int argc, char** argv) {
    std::filesystem::path root = util::getProgramLoc();
    m_childExec                = root / "psOffChild.exe";

    for (int i = 1; i < argc; ++i) {
      std::string_view arg(argv[i]);

      auto pred = [&arg](const char* pref) -> bool {
        // Check if argument is blacklisted
        return arg.starts_with(pref);
      };

      if (std::find_if(m_blacklist.begin(), m_blacklist.end(), pred) != m_blacklist.end()) continue;
      m_args += L" " + std::wstring(arg.begin(), arg.end());
    }

    restart();
  }

  void restart() {
    std::unique_lock const lock(m_accessProcInfo);

    if (m_procInfo.hProcess != INVALID_HANDLE_VALUE) {
      switch (WaitForSingleObject(m_procInfo.hProcess, INFINITE)) {
        case WAIT_OBJECT_0: break; // Process finished successfully
        case WAIT_FAILED: throw std::runtime_error(std::format("WaitForSingleObject failed: {}!", GetLastError()));
      }

      m_procInfo = {
          .hProcess    = INVALID_HANDLE_VALUE,
          .hThread     = INVALID_HANDLE_VALUE,
          .dwProcessId = 0,
          .dwThreadId  = 0,
      };
    }

    m_currCmdLine = m_childExec.c_str() + m_args;
    if (!CreateProcessW(m_childExec.c_str(), (wchar_t*)m_currCmdLine.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &m_startup, &m_procInfo)) {
      throw std::runtime_error(std::format("Failed to spawn child process: {}!", GetLastError()));
    }
  }

  bool isAlive(DWORD* ecode) {
    std::unique_lock const lock(m_accessProcInfo);
    return GetExitCodeProcess(m_procInfo.hProcess, ecode) && *ecode == STILL_ACTIVE;
  }
};

class EmuEventHandler: public events::system_circle::IEventHandler {
  std::vector<std::string> m_args = {};
  EmuProcess*              m_eproc;

  public:
  EmuEventHandler(EmuProcess* ep): m_eproc(ep) {};
  ~EmuEventHandler() = default;

  // ### Interface
  void onEventSetArguments(events::system_circle::SetArg const& data) override { m_args.push_back(data.arg); }

  void onEventLoadExec(events::system_circle::LoadArgs const& data) override {
    m_eproc->restart();

    /**
     * @brief Temporary hack!
     *
     * Wait til child process will be ready to
     * read the data and will not clear the
     * message queue
     *
     */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    events::system_cross::LoadArgs loadargs = {
        .mainExec   = data.mainExec,
        .mainRoot   = data.mainRoot,
        .updateRoot = data.updateRoot,
    };

    events::system_cross::postEventLoadExec(loadargs);

    for (auto it = m_args.begin(); it != m_args.end(); ++it) {
      events::system_cross::SetArg setarg = {.arg = *it};
      events::system_cross::postEventSetArguments(setarg);
    }

    events::system_cross::postEventRunExec();

    m_args.clear();
    printf("[MAINPROCESS] LoadExec sequence sent to new process!\n");
  }
};

int main(int argc, char** argv) {
  // Program params
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()
      // clang-format off
  ("help,h", "Help")
  ("d", "Wait for debugger")
  ("vkValidation", "Enable vulkan validation layers")
  ("bright", "use srgb display format (brightness)")
  ("4k", "try 4K display mode if game supports it")
  ("vsync", po::value<bool>()->default_value(true), "Enable vertical synchronization")
  ("file", po::value<std::string>(), "fullpath to applications binary")
  ("root", po::value<std::string>(), "Applications root")
  ("update", po::value<std::string>(), "update files folder")
      // clang-format on
      ;

  boost::program_options::variables_map m_vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), m_vm);
    po::notify(m_vm);
  } catch (...) {
    return -1;
  }

  if (m_vm.count("help")) {
    std::cout << desc << '\n';
    return -1;
  }
  // - program params

  events::system_cross::initChild();
  events::system_circle::initChild();

  EmuProcess      emuproc(argc, argv);
  EmuEventHandler emuev(&emuproc);

  events::system_cross::LoadArgs loadArgs {};
  loadArgs.mainExec   = m_vm.count("file") ? m_vm["file"].as<std::string>() : std::string();
  loadArgs.mainRoot   = m_vm.count("root") ? m_vm["root"].as<std::string>() : std::string();
  loadArgs.updateRoot = m_vm.count("update") ? m_vm["update"].as<std::string>() : std::string();

  events::system_cross::postEventLoadExec(loadArgs);
  events::system_cross::postEventRunExec();

  DWORD ecode;
  while (true) {
    if (!emuproc.isAlive(&ecode)) break;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  printf("[MAINPROCESS] Emulator's process gone!\n");
  return ecode;
}
