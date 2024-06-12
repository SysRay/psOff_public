#pragma once
#include <boost/serialization/serialization.hpp>
#include <cinttypes>
#include <string>
#include <vector>

namespace events::system {
struct LoadArgs {
  std::string mainExec;
  std::string mainRoot;
  std::string updateRoot;

  private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & mainExec;
    ar & mainRoot;
    ar & updateRoot;
  }
};

struct SetArg {
  std::string arg;

  private:
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & arg;
  }
};
} // namespace events::system
