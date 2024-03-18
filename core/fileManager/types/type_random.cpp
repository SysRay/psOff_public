#include "type_random.h"

class ITypeRandom: public IFile {
  public:
  ITypeRandom(): IFile(FileType::Device) {}

  virtual ~ITypeRandom() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<IFile> createType_random() {
  std::srand(std::time(nullptr));
  return std::make_unique<ITypeRandom>();
}

size_t ITypeRandom::read(void* buf, size_t nbytes) {
  auto cbuf = static_cast<char*>(buf);

  for (size_t i = 0; i < nbytes; i++)
    cbuf[i] = std::rand() & 0xFF;

  return nbytes;
}

size_t ITypeRandom::write(void* buf, size_t nbytes) {
  return 0;
}

int64_t ITypeRandom::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void ITypeRandom::sync() {}

bool ITypeRandom::isError() {
  return false;
}

void ITypeRandom::clearError() {}
