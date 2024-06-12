#include "events.h"

#include "intern/export.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <sstream>

// todo use boost archive

namespace events::system {
void initChild() {
  core::initChild();
}

void postEventLoadExec(LoadArgs const& data) {
  std::vector<char> cont;

  std::ostringstream              oss(std::ios::binary);
  boost::archive::binary_oarchive oa(oss);
  oa << (int32_t)0;
  oa << data;

  core::postEvent(oss.str());
}

void postEventSetArguments(SetArg const& data) {
  std::vector<char> cont;

  std::ostringstream              oss(std::ios::binary);
  boost::archive::binary_oarchive oa(oss);
  oa << (int32_t)1;
  oa << data;

  core::postEvent(oss.str());
}

void postEventRunExec() {
  std::ostringstream              oss(std::ios::binary);
  boost::archive::binary_oarchive oa(oss);
  oa << (int32_t)2;
  core::postEvent(oss.str());
}
} // namespace events::system
