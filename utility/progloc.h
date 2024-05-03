#pragma once

#ifndef BOOST_ALL_NO_LIB
#define BOOST_ALL_NO_LIB
#endif

#include <boost/dll/runtime_symbol_info.hpp>

namespace util {
inline std::wstring getProgramLoc() {
  boost::system::error_code ec;

  auto path = boost::dll::program_location(ec);
  if (ec.failed()) {
    printf("Failed to getProgramLoc(): %s", ec.message().c_str());
    exit(1);
  }

  path.remove_filename();
  return path.c_str();
}
} // namespace util
