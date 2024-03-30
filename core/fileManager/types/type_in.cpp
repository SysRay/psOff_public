#include "type_in.h"

#include "logging.h"
#include "modules_include/common.h"

LOG_DEFINE_MODULE(filesystem);

class TypeIn: public IFile {
  public:
  TypeIn(): IFile(FileType::Device) {}

  virtual ~TypeIn() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_in() {
  return std::make_unique<TypeIn>();
}

size_t TypeIn::read(void* buf, size_t nbytes) {
  if (nbytes == 0) return 0;
  printf("Emulator awaits your input: ");
  return std::fread(buf, 1, nbytes, stdin);
}

size_t TypeIn::write(void* buf, size_t nbytes) {
  return nbytes;
}

int64_t TypeIn::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void TypeIn::sync() {}