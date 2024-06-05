#define __APICALL_INITPARAMS_EXTERN
#include "initParams.h"
#undef __APICALL_INITPARAMS_EXTERN
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>

class InitParams: public IInitParams {
  boost::program_options::variables_map m_vm;

  public:
  InitParams() = default;

  bool init(int argc, char** argv) final;
  bool isDebug() final;

  std::string getApplicationPath() final;
  std::string getApplicationRoot() final;
  std::string getPipeName() final;
  std::string getUpdateRoot() final;

  bool enableValidation() final;
  bool enableBrightness() final;
  bool useVSYNC() final;
  bool try4K() final;

  virtual ~InitParams() = default;
};

bool InitParams::init(int argc, char** argv) {
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

  try {
    po::store(po::parse_command_line(argc, argv, desc), m_vm);
    po::notify(m_vm);
  } catch (...) {
    return false;
  }

  if (m_vm.count("help")) {
    std::cout << desc << '\n';
    return false;
  }
  if (m_vm.count("file") == 0) {
    std::cout << "--file missing\n";
    return false;
  }

  return true;
}

IInitParams* accessInitParams() {
  static InitParams obj;
  return &obj;
}

bool InitParams::isDebug() {
  return m_vm.count("d");
}

std::string InitParams::getApplicationPath() {
  return m_vm.count("file") ? m_vm["file"].as<std::string>() : std::string();
}

std::string InitParams::getApplicationRoot() {
  return m_vm.count("root") ? m_vm["root"].as<std::string>() : std::string();
}

std::string InitParams::getUpdateRoot() {
  return m_vm.count("update") ? m_vm["update"].as<std::string>() : std::string();
}

std::string InitParams::getPipeName() {
  return m_vm.count("pipe") ? m_vm["pipe"].as<std::string>() : std::string();
}

bool InitParams::enableValidation() {
  return m_vm.count("vkValidation");
}

bool InitParams::enableBrightness() {
  return m_vm.count("bright");
}

bool InitParams::useVSYNC() {
  return m_vm["vsync"].as<bool>();
}

bool InitParams::try4K() {
  return m_vm.count("4k");
}
