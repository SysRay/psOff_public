#include "type_null.h"

#include "modules_include/common.h"

class TypeNull: public IFile {
  public:
  TypeNull(): IFile(FileType::Device) {}

  virtual ~TypeNull() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_null() {
  return std::make_unique<TypeNull>();
}

size_t TypeNull::read(void* buf, size_t nbytes) {
  return nbytes;
}

size_t TypeNull::write(void* buf, size_t nbytes) {
  return nbytes;
}

int64_t TypeNull::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void TypeNull::sync() {}