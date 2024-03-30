#include "type_out.h"

#include "logging.h"

LOG_DEFINE_MODULE(filesystem);

class TypeOut: public IFile {
  const SceFileOutChannel m_channel = SCE_TYPEOUT_ERROR;

  public:
  TypeOut(SceFileOutChannel ch): IFile(FileType::Device), m_channel(ch) {}

  virtual ~TypeOut() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_out(SceFileOutChannel ch) {
  return std::make_unique<TypeOut>(ch);
}

size_t TypeOut::read(void* buf, size_t nbytes) {
  return 0;
}

size_t TypeOut::write(void* buf, size_t nbytes) {
  LOG_USE_MODULE(filesystem);

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

int64_t TypeOut::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void TypeOut::sync() {}
