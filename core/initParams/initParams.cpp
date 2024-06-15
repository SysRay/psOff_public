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

  return true;
}

IInitParams* accessInitParams() {
  static InitParams obj;
  return &obj;
}

bool InitParams::isDebug() {
  return m_vm.count("d");
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
