#include "eventsystem\events\system_cross\events.h"
#include "utility/progloc.h"

#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <thread>
#include <windows.h>

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

  STARTUPINFOW sti = {
      .cb = sizeof(STARTUPINFOA),
  };
  PROCESS_INFORMATION pi;

  std::filesystem::path root = util::getProgramLoc();

  auto childExec = root / "psOffChild.exe";

  std::wstring args;

  std::vector<const char*> blacklist = {"--file", "--update", "--root"};

  for (int i = 1; i < argc; ++i) {
    std::string_view arg(argv[i]);

    auto pred = [&arg](const char* pref) -> bool {
      // Check if argument is blacklisted
      return arg.starts_with(pref);
    };

    if (std::find_if(blacklist.begin(), blacklist.end(), pred) != blacklist.end()) continue;
    args += L" " + std::wstring(arg.begin(), arg.end());
  }

  std::wstring commandLine = childExec.c_str() + args;
  if (!CreateProcessW(childExec.c_str(), (wchar_t*)commandLine.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &sti, &pi)) {
    printf("Failed to spawn child process: %lu!\n", GetLastError());
    return -2;
  }

  events::system_cross::LoadArgs loadArgs {};
  loadArgs.mainExec   = m_vm.count("file") ? m_vm["file"].as<std::string>() : std::string();
  loadArgs.mainRoot   = m_vm.count("root") ? m_vm["root"].as<std::string>() : std::string();
  loadArgs.updateRoot = m_vm.count("update") ? m_vm["update"].as<std::string>() : std::string();

  events::system_cross::postEventLoadExec(loadArgs);
  events::system_cross::postEventRunExec();

  DWORD ecode;
  while (true) {
    if (GetExitCodeProcess(pi.hProcess, &ecode) && ecode != STILL_ACTIVE) break;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  printf("Emulator's process gone!\n");
  return ecode;
}
