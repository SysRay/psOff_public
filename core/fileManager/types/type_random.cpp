#include "type_random.h"

std::unique_ptr<TypeRandom> createType_random() {
  std::srand(std::time(nullptr));
  return std::make_unique<TypeRandom>();
}

size_t TypeRandom::read(void* buf, size_t nbytes) {
  auto cbuf = static_cast<char*>(buf);

  for (size_t i = 0; i < nbytes; i++)
    cbuf[i] = std::rand() & 0xFF;

  return nbytes;
}

size_t TypeRandom::write(void* buf, size_t nbytes) {
  return 0;
}

int64_t TypeRandom::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

bool TypeRandom::isError() {
  return false;
}

void TypeRandom::clearError() {}
