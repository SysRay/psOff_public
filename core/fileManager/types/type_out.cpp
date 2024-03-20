#include "type_out.h"

#include "logging.h"

LOG_DEFINE_MODULE(filesystem);

class ITypeOut: public IFile {
  const SceFileOutChannel m_channel = SCE_ITYPEOUT_ERROR;

  public:
  ITypeOut(SceFileOutChannel ch): IFile(FileType::Device), m_channel(ch) {}

  virtual ~ITypeOut() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<IFile> createType_out(SceFileOutChannel ch) {
  std::srand(std::time(nullptr));
  return std::make_unique<ITypeOut>(ch);
}

size_t ITypeOut::read(void* buf, size_t nbytes) {
  return 0;
}

size_t ITypeOut::write(void* buf, size_t nbytes) {
  LOG_USE_MODULE(filesystem);

  std::string str((const char*)buf, nbytes);
  while (str.back() == '\n')
    str.pop_back();

  switch (m_channel) {
    case SCE_ITYPEOUT_ERROR: LOG_ERR(L"%S", str.c_str()); break;

    case SCE_ITYPEOUT_TRACE: LOG_TRACE(L"%S", str.c_str()); break;

    default: LOG_CRIT(L"Unknown channel: %d", (uint32_t)m_channel); break;
  }

  return nbytes;
}

int64_t ITypeOut::lseek(int64_t offset, SceWhence whence) {
  return -1;
}

void ITypeOut::sync() {}

bool ITypeOut::isError() {
  return false;
}

void ITypeOut::clearError() {}
