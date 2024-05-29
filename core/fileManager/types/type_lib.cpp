#include "type_lib.h"

#include "core/runtime/runtimeLinker.h"
#include "logging.h"

LOG_DEFINE_MODULE(IODevice_LIB);

class TypeLib: public IFile {
  Program* m_prog;

  public:
  TypeLib(Program* prog): IFile(FileType::Library), m_prog(prog) {}

  ~TypeLib() { accessRuntimeLinker().stopModule(m_prog->id); }

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  void    sync() final;
  int     ioctl(int request, SceVariadicList argp) final;
  int     fcntl(int cmd, SceVariadicList argp) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_lib(Program* prog) {
  return std::make_unique<TypeLib>(prog);
}

size_t TypeLib::read(void* buf, size_t nbytes) {
  LOG_USE_MODULE(IODevice_LIB);
  LOG_CRIT(L"Program(%p)->read(%p, %llu)", m_prog->id, buf, nbytes);
  return 0;
}

size_t TypeLib::write(void* buf, size_t nbytes) {
  LOG_USE_MODULE(IODevice_LIB);
  LOG_CRIT(L"Program(%p)->write(%p, %llu)", m_prog->id, buf, nbytes);
  return 0;
}

void TypeLib::sync() {}

int TypeLib::ioctl(int request, SceVariadicList argp) {
  return 0;
}

int TypeLib::fcntl(int cmd, SceVariadicList argp) {
  return 0;
}

int64_t TypeLib::lseek(int64_t offset, SceWhence whence) {
  return -1;
}
