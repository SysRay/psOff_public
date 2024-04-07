#include "type_file.h"

#include "logging.h"

#include <filesystem>
#include <fstream>
#include <windows.h>

LOG_DEFINE_MODULE(File);

namespace {
typedef HANDLE Handle_t;

std::pair<Handle_t, int32_t> open(std::filesystem::path path, filesystem::SceOpen mode) {
  LOG_USE_MODULE(File);

  DWORD shareMode = 0;
  DWORD access    = 0;

  // Set acces and shareMode;
  switch (mode.mode) {
    case filesystem::SceOpenMode::RDONLY: {
      access = GENERIC_READ;
      if (!mode.exlock) shareMode = FILE_SHARE_READ;
    } break;
    case filesystem::SceOpenMode::WRONLY: {
      access = GENERIC_WRITE;
      if (!mode.exlock) shareMode = FILE_SHARE_WRITE;
    } break;
    case filesystem::SceOpenMode::RDWR: {
      access = GENERIC_READ | GENERIC_WRITE;
      if (!mode.exlock) shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    } break;
  }
  // -

  DWORD flags = OPEN_EXISTING;
  if (mode.trunc) flags = CREATE_ALWAYS;
  // Create new if it doesn't exist
  else if (mode.create) {
    flags = CREATE_NEW;
  }

  auto pathString = path.wstring();
  std::replace(pathString.begin(), pathString.end(), L'/', L'\\');
  HANDLE file = CreateFileW(pathString.c_str(), access, shareMode, 0, flags, FILE_ATTRIBUTE_NORMAL, 0);
  if (file == INVALID_HANDLE_VALUE) {
    auto const lastErr = GetLastError();
    LOG_WARN(L"open file: %s flags:0x%lx access:0x%lx shareMode:0x%lx mode:0x%llx(%u) error:0x%lx ", pathString.c_str(), flags, access, shareMode,
             *(uint32_t*)(&mode), mode.mode, lastErr);
    if (lastErr == 0x57) return {nullptr, (int)ErrCode::_EINVAL};
    if (lastErr == 0x20) return {nullptr, (int)ErrCode::_EALREADY};
    return {nullptr, Ok};
  }

  LOG_DEBUG(L"[%lld] opened file: %s flags:0x%lx access:0x%lx shareMode:0x%lx mode:0x%lx(%u)", file, pathString.c_str(), flags, access, shareMode,
            *(uint32_t*)(&mode), mode.mode);
  return {file, Ok};
}
} // namespace

class TypeFile: public IFile {
  Handle_t m_file = INVALID_HANDLE_VALUE;

  filesystem::SceOpen const m_mode;

  bool m_isAppend = false;

  public:
  TypeFile(std::filesystem::path path, filesystem::SceOpen mode): IFile(FileType::File), m_mode(mode) {
    auto [file, err] = open(path, mode);
    m_file           = file;

    m_errCode = err;

    m_isAppend = mode.append;
  }

  virtual ~TypeFile() {
    if (m_file != INVALID_HANDLE_VALUE) {
      CloseHandle(m_file);
      sync();
    }
  }

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  void    sync() final;
  int     ioctl(int request, SceVariadicList argp) final;
  int     fcntl(int cmd, SceVariadicList argp) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;

  virtual void* getNative() final { return m_file; }
};

std::unique_ptr<IFile> createType_file(std::filesystem::path path, filesystem::SceOpen mode) {
  return std::make_unique<TypeFile>(path, mode);
}

size_t TypeFile::read(void* buf, size_t nbytes) {
  DWORD numRead = 0;
  if (!ReadFile(m_file, buf, nbytes, &numRead, nullptr)) {
    auto const lastError = GetLastError();
    if (lastError != ERROR_IO_PENDING) {
      LOG_USE_MODULE(File);
      LOG_WARN(L"Read error: 0x%lx", GetLastError());
      m_errCode = (int)ErrCode::_EIO;

      return 0;
    }
  }
  return numRead;
}

size_t TypeFile::write(void* buf, size_t nbytes) {
  // The file is opened in append mode.  Before each write(2),
  // the file offset is positioned at the end of the file
  if (m_isAppend) {
    lseek(0, SceWhence::end);
  }
  // -

  DWORD numWrite = 0;
  if (!WriteFile(m_file, buf, nbytes, &numWrite, nullptr)) {
    LOG_USE_MODULE(File);
    LOG_WARN(L"Write error: 0x%lx", GetLastError());
    m_errCode = (int)ErrCode::_EIO;
    return 0;
  }
  return numWrite;
}

void TypeFile::sync() {
  FlushFileBuffers(m_file);
}

int TypeFile::ioctl(int request, SceVariadicList argp) {
  return 0;
}

int TypeFile::fcntl(int cmd, SceVariadicList argp) {
  return 0;
}

int64_t TypeFile::lseek(int64_t offset, SceWhence whence) {
  static int _whence[] = {
      FILE_BEGIN,
      FILE_CURRENT,
      FILE_END,
  };

  LONG  upperHalf = offset >> 32;
  DWORD lowerHalf = SetFilePointer(m_file, (uint32_t)offset, &upperHalf, _whence[(int)whence]);

  return ((uint64_t)upperHalf << 32) | lowerHalf;
}
