#define __APICALL_EXTERN
#include "initParams.h"
#undef __APICALL_EXTERN
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>

struct InitParamsPimpl {
  boost::program_options::variables_map m_vm;
  InitParamsPimpl() = default;
};

InitParams::InitParams() {
  _pImpl = std::make_unique<InitParamsPimpl>().release();
}

InitParams::~InitParams() {
  delete _pImpl;
}

bool InitParams::init(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()
      // clang-format off
  ("help,h", "Help")
  ("d", "Wait for debugger")
  ("vkValidation", "Enable vulkan validation layers")
  ("vsync", po::value<bool>()->default_value(true), "Enable vulkan validation layers")
  ("file", po::value<std::string>(), "fullpath to applications binary")
  ("root", po::value<std::string>(), "Applications root")
  ("update", po::value<std::string>(), "update files folder")
      // clang-format on
      ;

  auto& vm = _pImpl->m_vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << '\n';
    return false;
  }
  if (!vm.count("file")) {
    std::cout << "--file missing\n";
    return false;
  }

  return true;
}

InitParams* accessInitParams() {
  static InitParams obj;
  return &obj;
}

bool InitParams::isDebug() {
  return _pImpl->m_vm.count("d");
}

std::string InitParams::getApplicationPath() {
  return _pImpl->m_vm.count("file") ? _pImpl->m_vm["file"].as<std::string>() : std::string();
}

std::string InitParams::getApplicationRoot() {
  return _pImpl->m_vm.count("root") ? _pImpl->m_vm["root"].as<std::string>() : std::string();
}

std::string InitParams::getUpdateRoot() {
  return _pImpl->m_vm.count("update") ? _pImpl->m_vm["update"].as<std::string>() : std::string();
}

bool InitParams::enableValidation() {
  return _pImpl->m_vm.count("vkValidation");
}

bool InitParams::useVSYNC() {
  return _pImpl->m_vm["vsync"].as<bool>();
}
