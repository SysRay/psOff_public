#include "logging.h"
#include "type_random.h"

LOG_DEFINE_MODULE(IODevice_RNG);

class TypeRNG: public IFile {
  public:
  TypeRNG(): IFile(FileType::Device) {}

  virtual ~TypeRNG() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  void    sync() final;
  int     ioctl(int request, SceVariadicList argp) final;
  int     fcntl(int cmd, SceVariadicList argp) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;

  void* getNative() final { return nullptr; }
};

std::unique_ptr<IFile> createType_rng() {
  std::srand(std::time(nullptr));
  return std::make_unique<TypeRNG>();
}

size_t TypeRNG::read(void* buf, size_t nbytes) {
  auto cbuf = static_cast<char*>(buf);

  for (size_t i = 0; i < nbytes; i++)
    cbuf[i] = std::rand() & 0xFF;

  return nbytes;
}

size_t TypeRNG::write(void* buf, size_t nbytes) {
  return 0;
}

void TypeRNG::sync() {}

int TypeRNG::ioctl(int request, SceVariadicList argp) {
  LOG_USE_MODULE(IODevice_RNG);

  switch (request) {
    // These are unknown for now
    case 0x40445301: break;
    case 0x40445302: break;

    default: LOG_ERR(L"Unknown ioctl request to /dev/rng: %d", request); return -1;
  }

  return 0;
}

int TypeRNG::fcntl(int cmd, SceVariadicList argp) {
  return 0;
}

int64_t TypeRNG::lseek(int64_t offset, SceWhence whence) {
  return -1;
}
