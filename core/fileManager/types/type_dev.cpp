#include "type_dev.h"
#include "type_random.h"

std::unique_ptr<TypeDev> createType_dev(std::filesystem::path path, std::ios_base::openmode mode) {
  // todo: /dev/rng?
  if (path == "/dev/urandom" || path == "/dev/urandom") {
    return createType_random();
  } else { // todo: other devices
    throw std::runtime_error("Unknown device!");
  }

  return {};
}

void TypeDev::sync() {}
