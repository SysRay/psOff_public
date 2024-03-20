#include "type_null.h"

#include "modules_include/common.h"

class ITypeNull: public IFile {
  public:
  ITypeNull(): IFile(FileType::Device) {}

  virtual ~ITypeNull() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<IFile> createType_null() {
  return std::make_unique<ITypeNull>();
}

size_t ITypeNull::read(void* buf, size_t nbytes) {
  return nbytes;
}

size_t ITypeNull::write(void* buf, size_t nbytes) {
  return nbytes;
}

int64_t ITypeNull::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void ITypeNull::sync() {}

bool ITypeNull::isError() {
  return false;
}

void ITypeNull::clearError() {}
