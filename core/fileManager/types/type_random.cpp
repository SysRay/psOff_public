#include "type_random.h"

class TypeRandom: public IFile {
  public:
  TypeRandom(): IFile(FileType::Device) {}

  virtual ~TypeRandom() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_random() {
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

void TypeRandom::sync() {}
