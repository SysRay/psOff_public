#include "eventsystem\events\system\events.h"

#include <boost/program_options.hpp>
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
  ("pipe", po::value<std::string>(), "Communication pipe name")
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

  events::system::initChild();

  STARTUPINFOA sti = {
      .cb = sizeof(STARTUPINFOA),
  };
  PROCESS_INFORMATION pi;

  std::string procArgs = "";
  if (!CreateProcessA(".\\psOffChild.exe", (char*)procArgs.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &sti, &pi)) {
    printf("Failed to spawn child process: %lu!\n", GetLastError());
    return -2;
  }

  events::system::LoadArgs loadArgs {};
  loadArgs.mainExec   = m_vm.count("file") ? m_vm["file"].as<std::string>() : std::string();
  loadArgs.mainRoot   = m_vm.count("root") ? m_vm["root"].as<std::string>() : std::string();
  loadArgs.updateRoot = m_vm.count("update") ? m_vm["update"].as<std::string>() : std::string();

  events::system::postEventLoadExec(loadArgs);
  events::system::postEventRunExec();

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}