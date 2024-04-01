#include "type_out.h"

#include "logging.h"

LOG_DEFINE_MODULE(IODevice_OUT);

class TypeOut: public IFile {
  const SceFileOutChannel m_channel = SCE_TYPEOUT_ERROR;

  public:
  TypeOut(SceFileOutChannel ch): IFile(FileType::Device), m_channel(ch) {}

  virtual ~TypeOut() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  void    sync() final;
  int     ioctl(int request, SceVariadicList argp) final;
  int     fcntl(int cmd, SceVariadicList argp) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_out(SceFileOutChannel ch) {
  return std::make_unique<TypeOut>(ch);
}

size_t TypeOut::read(void* buf, size_t nbytes) {
  return 0;
}

size_t TypeOut::write(void* buf, size_t nbytes) {
  LOG_USE_MODULE(IODevice_OUT);

  std::string str((const char*)buf, nbytes);
  while (str.back() == '\n')
    str.pop_back();

  switch (m_channel) {
    case SCE_TYPEOUT_ERROR: {
      printf("Console:%s\n", str.c_str());
      LOG_DEBUG(L"%S", str.c_str());

    } break;

    case SCE_TYPEOUT_DEBUG: LOG_DEBUG(L"%S", str.c_str()); break;

    default: LOG_CRIT(L"Unknown channel: %d", (uint32_t)m_channel); break;
  }

  return nbytes;
}

void TypeOut::sync() {}

int TypeOut::ioctl(int request, SceVariadicList argp) {
  return 0;
}

int TypeOut::fcntl(int cmd, SceVariadicList argp) {
  return 0;
}

int64_t TypeOut::lseek(int64_t offset, SceWhence whence) {
  return -1;
}
