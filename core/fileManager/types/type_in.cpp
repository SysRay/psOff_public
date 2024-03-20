#include "type_in.h"
#include "logging.h"
#include "modules_include/common.h"

LOG_DEFINE_MODULE(filesystem);

class ITypeIn: public IFile {
  public:
  ITypeIn(): IFile(FileType::Device) {}

  virtual ~ITypeIn() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<IFile> createType_in() {
  std::srand(std::time(nullptr));
  return std::make_unique<ITypeIn>();
}

size_t ITypeIn::read(void* buf, size_t nbytes) {
  if (nbytes == 0) return 0;
  printf("Emulator awaits your input: ");
  return ::fread(buf, 1, nbytes, stdin);
}

size_t ITypeIn::write(void* buf, size_t nbytes) {
  return nbytes;
}

int64_t ITypeIn::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void ITypeIn::sync() {}

bool ITypeIn::isError() {
  return false;
}

void ITypeIn::clearError() {}
