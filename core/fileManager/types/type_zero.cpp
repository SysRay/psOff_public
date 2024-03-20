#include "type_zero.h"

class ITypeZero: public IFile {
  public:
  ITypeZero(): IFile(FileType::Device) {}

  virtual ~ITypeZero() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<IFile> createType_zero() {
  return std::make_unique<ITypeZero>();
}

size_t ITypeZero::read(void* buf, size_t nbytes) {
  for (size_t i = 0; i < nbytes; i++) {
    ((char*)buf)[i] = '\0';
  }

  return nbytes;
}

size_t ITypeZero::write(void* buf, size_t nbytes) {
  return 0;
}

int64_t ITypeZero::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void ITypeZero::sync() {}

bool ITypeZero::isError() {
  return false;
}

void ITypeZero::clearError() {}
