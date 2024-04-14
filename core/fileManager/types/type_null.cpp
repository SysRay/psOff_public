#include "type_null.h"

#include "modules_include/common.h"

class TypeNull: public IFile {
  public:
  TypeNull(): IFile(FileType::Device) {}

  virtual ~TypeNull() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  void    sync() final;
  int     ioctl(int request, SceVariadicList argp) final;
  int     fcntl(int cmd, SceVariadicList argp) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_null() {
  return std::make_unique<TypeNull>();
}

size_t TypeNull::read(void* buf, size_t nbytes) {
  return -1;
}

size_t TypeNull::write(void* buf, size_t nbytes) {
  return nbytes;
}

void TypeNull::sync() {}

int TypeNull::ioctl(int request, SceVariadicList argp) {
  return -1;
}

int TypeNull::fcntl(int cmd, SceVariadicList argp) {
  return -1;
}

int64_t TypeNull::lseek(int64_t offset, SceWhence whence) {
  return -1;
}
