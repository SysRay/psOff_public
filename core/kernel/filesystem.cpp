#include "filesystem.h"

#include "core/dmem/dmem.h"
#include "core/fileManager/fileManager.h"
#include "core/fileManager/types/type_file.h"
#include "core/fileManager/types/type_random.h"
#include "core/memory/memory.h"
#include "logging.h"

#include <assert.h>
#include <unordered_map>
#include <windows.h>

LOG_DEFINE_MODULE(filesystem);

namespace {
std::pair<uint32_t, uint32_t> convProtection(int prot) {
  switch (prot & 0xf) {
    case 0: return {PAGE_NOACCESS, 0};
    case 1: return {PAGE_READONLY, FILE_MAP_READ};
    case 2:
    case 3: return {PAGE_READWRITE, FILE_MAP_ALL_ACCESS};
    case 4: return {PAGE_EXECUTE, FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE};
    case 5: return {PAGE_EXECUTE_READ, FILE_MAP_EXECUTE | FILE_MAP_READ};
    case 6:
    case 7: return {PAGE_EXECUTE_READWRITE, FILE_MAP_ALL_ACCESS};
  }

  return {PAGE_NOACCESS, 0};
}

std::unique_ptr<IFile> createType_dev(std::filesystem::path path, std::ios_base::openmode mode) {
  LOG_USE_MODULE(filesystem);

  // todo: /dev/rng? No ioctl for now
  if (path == "/dev/urandom" || path == "/dev/urandom") {
    return createType_random();
  } else { // todo: other devices
    LOG_CRIT(L"%S: unknown device!", path.c_str());
  }

  return {};
}

int open_dev(const char* path, filesystem::SceOpen flags, filesystem::SceKernelMode kernelMode) {
  LOG_USE_MODULE(filesystem);

  std::ios_base::openmode mode = std::ios::binary;

  switch (flags.mode) {
    case filesystem::SceOpenMode::RDONLY: mode |= std::ios::in; break;
    case filesystem::SceOpenMode::WRONLY: mode |= std::ios::out; break;
    case filesystem::SceOpenMode::RDWR: mode |= std::ios::out | std::ios::in; break;
  }

  auto      file   = createType_dev(path, mode);
  int const handle = accessFileManager().addFile(std::move(file), path);
  LOG_INFO(L"OpenFile[%d]: %s mode:0x%lx(0x%lx)", handle, path, mode, kernelMode);
  return handle;
}
} // namespace

namespace filesystem {
int mmap(void* addr, size_t len, int prot, SceMap flags, int fd, int64_t offset, void** res) {
  LOG_USE_MODULE(filesystem);

  // Mapping cases
  if (flags.mode == SceMapMode::FIXED) {
    LOG_ERR(L"todo: Mmap fixed 0x%08llx len:0x%08llx prot:%d flags:%d fd:%d offset:%lld", addr, len, prot, flags, fd, offset);
    return getErr(ErrCode::_EINVAL);
  }

  if (flags.mode == SceMapMode::VOID_) {
    // reserve memory
    LOG_ERR(L"todo: Mmap void 0x%08llx len:0x%08llx prot:%d flags:%d fd:%d offset:%lld", addr, len, prot, flags, fd, offset);
    return getErr(ErrCode::_EINVAL);
  }

  if (flags.type == SceMapType::ANON) {
    *res = (void*)accessFlexibleMemory().alloc((uint64_t)addr, len, prot); // registers mapping (flexible)

    LOG_DEBUG(L"Mmap anon addr:0x%08llx len:0x%08llx prot:%d flags:%d fd:%d offset:%lld -> out:0x%08llx", addr, len, prot, flags, fd, offset, *res);
    return getErr(ErrCode::_EINVAL);
  }
  // -

  // Do file mapping
  if (fd < FILE_DESCRIPTOR_MIN) {
    LOG_DEBUG(L"Mmap error addr:0x%08llx len:0x%08llx prot:%d flags:%08llx fd:%d offset:%lld -> out:0x%08llx", addr, len, prot, flags, fd, offset, *res);
    return getErr(ErrCode::_EPERM);
  }

  auto const [fileProt, viewProt] = convProtection(prot);

  auto file = accessFileManager().accessFile(fd);

  if (file->getNative() == nullptr) {
    LOG_ERR(L"Mmap fd:%d no nativeHandle", fd);
    return getErr(ErrCode::_EMFILE);
  }

  uint64_t mappingLength = (fileProt == PAGE_READONLY) || (fileProt == PAGE_EXECUTE_READ) ? 0 : len;
  auto     lenSplit      = std::bit_cast<std::array<uint32_t, 2>>(mappingLength);

  HANDLE hFileMapping = CreateFileMapping(file->getNative(),
                                          NULL, // default security
                                          fileProt,
                                          lenSplit[1], // maximum object size (high-order DWORD)
                                          lenSplit[0], // maximum object size (low-order DWORD)
                                          NULL);       // name of mapping object

  *res = nullptr;

  if (hFileMapping == NULL) {
    LOG_ERR(L"Mmap CreateFileMapping == NULL for| 0x%08llx len:0x%08llx prot:%d flags:%0lx fd:%d offset:%lld err:%04x", addr, mappingLength, prot, flags, fd,
            offset, GetLastError());
  } else {
    auto offsetSplit = std::bit_cast<std::array<uint32_t, 2>>(offset);
    *res             = MapViewOfFile(hFileMapping,   // handle to file mapping object
                                     viewProt,       // read/write permission
                                     offsetSplit[1], // high offset
                                     offsetSplit[0], // low offset
                                     mappingLength); // number of bytes to map
    if (*res == NULL) {
      LOG_ERR(L"Mmap MapViewOfFile == NULL for| 0x%08llx len:0x%08llx prot:%d flags:%0lx fd:%d offset:%lld err:%04x", addr, mappingLength, prot, flags, fd,
              offset, GetLastError());
    } else {
      LOG_DEBUG(L"Mmap addr:0x%08llx len:0x%08llx prot:%d flags:%0lx fd:%d offset:%lld -> out:0x%08llx", addr, mappingLength, prot, flags, fd, offset, *res);
      registerMapping((uint64_t)*res, MappingType::File);
    }
  }

  CloseHandle(hFileMapping); // is kept open internally until mapView is unmapped

  if (*res == nullptr) {
    return getErr(ErrCode::_EACCES);
  }
  return Ok;
}

int munmap(void* addr, size_t len) {
  LOG_USE_MODULE(filesystem);

  // unregister and then delete
  auto type = unregisterMapping((uint64_t)addr);

  switch (type) {
    case MappingType::File: {
      return UnmapViewOfFile(addr) != 0 ? Ok : -1;
    } break;

    case MappingType::Flexible: {
      return accessFlexibleMemory().destroy((uint64_t)addr, len) != 0 ? Ok : -1;
    } break;

    case MappingType::Fixed: {
      LOG_ERR(L"tod munmap Fixed 0x%08llx 0x%08llx", addr, len);
    } break;

    case MappingType::None: {
      LOG_ERR(L"munmap unkown 0x%08llx 0x%08llx", addr, len);
    } break;
  }
  return -1;
}

size_t read(int handle, void* buf, size_t nbytes) {
  LOG_USE_MODULE(filesystem);

  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    return getErr(ErrCode::_EBADF);
  }

  auto const count = file->read((char*)buf, nbytes);
  LOG_TRACE(L"KernelRead[%d]: 0x%08llx:%llu read(%lld)", handle, (uint64_t)buf, nbytes, count);
  return count;
}

int64_t write(int handle, const void* buf, size_t nbytes) {
  LOG_USE_MODULE(filesystem);

  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    LOG_ERR(L"KernelWrite[%d] file==nullptr: 0x%08llx:%llu", handle, (uint64_t)buf, nbytes);
    return getErr(ErrCode::_EBADF);
  }

  size_t const count = file->write((char*)buf, nbytes);

  LOG_TRACE(L"KernelWrite[%d]: 0x%08llx:%llu count:%llu", handle, (uint64_t)buf, nbytes, count);

  if (auto err = file->getErr(); err != Ok) {
    return getErr((ErrCode)err);
  }
  return count;
}

int open(const char* path, SceOpen flags, SceKernelMode kernelMode) {
  LOG_USE_MODULE(filesystem);

  if (path == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  if (std::string_view(path).starts_with("/dev")) {
    return open_dev(path, flags, kernelMode);
  }

  auto mapped = accessFileManager().getMappedPath(path);
  if (!mapped) {
    return getErr(ErrCode::_EACCES);
  }
  auto const mappedPath = mapped.value();

  // handle excl (file already exists)
  if (flags.excl && flags.create && std::filesystem::exists(mappedPath)) {
    return getErr(ErrCode::_EEXIST);
  }
  // -

  bool isDir = std::filesystem::is_directory(mappedPath);

  if (flags.directory || isDir) {
    if (!isDir) {
      LOG_WARN(L"Directory doesn't exist: %s", mappedPath.c_str());
      return getErr(ErrCode::_ENOTDIR);
    }

    if (!std::filesystem::exists(mappedPath.parent_path())) std::filesystem::create_directories(mappedPath);

    auto      dirIt  = std::make_unique<std::filesystem::directory_iterator>(mappedPath);
    int const handle = accessFileManager().addDirIterator(std::move(dirIt), mappedPath);
    LOG_INFO(L"OpenDir [%d]: %S (%s)", handle, path, mappedPath.c_str());

    return handle;
  } else {
    // error if read only input file does not exist
    if (flags.mode == SceOpenMode::RDONLY && !std::filesystem::exists(mappedPath)) {
      LOG_WARN(L"File doesn't exist: %s mode:0x%lx", mappedPath.c_str(), flags.mode);
      return getErr(ErrCode::_ENOENT);
    }

    auto file = createType_file(mappedPath, flags);
    if (auto err = file->getErr(); err != Ok) {
      return getErr((ErrCode)err);
    }
    int const handle = accessFileManager().addFile(std::move(file), mappedPath);
    LOG_INFO(L"OpenFile[%d]: %s mode:0x%lx(0x%lx)", handle, mappedPath.c_str(), *(int*)&flags, kernelMode);

    return handle;
  }
}

int close(int handle) {
  LOG_USE_MODULE(filesystem);
  LOG_INFO(L"Closed[%d]", handle);
  if (handle < FILE_DESCRIPTOR_MIN) {
    return getErr(ErrCode::_EPERM);
  }

  accessFileManager().remove(handle);
  return Ok;
}

int unlink(const char* path) {
  LOG_USE_MODULE(filesystem);
  if (path == nullptr) {
    return getErr(ErrCode::_EINVAL);
  }

  auto const _mapped = accessFileManager().getMappedPath(path);
  if (!_mapped) {
    return getErr(ErrCode::_EACCES);
  }
  auto const mapped = _mapped.value();

  if (std::filesystem::is_directory(mapped)) {
    return getErr(ErrCode::_EPERM);
  }

  if (std::filesystem::remove(mapped)) {
    LOG_INFO(L"Deleted: %S", path);
    return Ok;
  }

  return getErr(ErrCode::_ENOENT);
}

int chmod(const char* path, SceKernelMode mode) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"TODO %S", __FUNCTION__);
  return Ok;
}

int checkReachability(const char* path) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S %S", __FUNCTION__, path);
  auto mapped = accessFileManager().getMappedPath(path);
  if (!mapped) {
    return getErr(ErrCode::_EACCES);
  }
  auto const mappedPath = mapped.value();

  if (std::filesystem::exists(mappedPath.parent_path())) return Ok;

  return getErr(ErrCode::_EACCES);
}

void sync(void) {
  // todo: sync all open files?
}

int fsync(int handle) {
  LOG_USE_MODULE(filesystem);
  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    LOG_ERR(L"KernelFsync[%d]", handle);
    return getErr(ErrCode::_EBADF);
  }

  file->sync();
  return Ok;
}

int fdatasync(int fd) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S %d", __FUNCTION__, fd);
  return Ok;
}

int fcntl(int fd, int cmd, va_list args) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S %d", __FUNCTION__, fd);
  return Ok;
}

size_t readv(int handle, const SceKernelIovec* iov, int iovcnt) {
  LOG_USE_MODULE(filesystem);

  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    return getErr(ErrCode::_EBADF);
  }

  // todo: move it to IFile?
  size_t count = 0;

  for (int n = 0; n < iovcnt; ++n) {
    auto* item = &iov[n];

    if (item->iov_base == nullptr || item->iov_len == 0) continue;

    auto const countTemp = file->read((char*)item->iov_base, item->iov_len);

    LOG_TRACE(L"KernelRead[%d]: 0x%08llx:%llu read(%lld)", handle, (uint64_t)item->iov_base, item->iov_len, countTemp);

    count += countTemp;
    if (auto err = file->getErr(); err != Ok) {
      LOG_TRACE(L"file end");
      break;
    }
  }

  return count;
}

size_t writev(int handle, const SceKernelIovec* iov, int iovcnt) {
  LOG_USE_MODULE(filesystem);

  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    return getErr(ErrCode::_EBADF);
  }

  // todo: move it to IFile?
  size_t count = 0;

  for (int n = 0; n < iovcnt; n++) {
    auto* item = &iov[n];

    if (item->iov_base == nullptr || item->iov_len == 0) continue;

    auto const countTemp = file->write(item->iov_base, item->iov_len);

    LOG_TRACE(L"KernelWrite[%d]: 0x%08llx:%llu write(%lld)", handle, (uint64_t)item->iov_base, item->iov_len, countTemp);

    count += countTemp;

    if (auto err = file->getErr(); err != Ok) {
      LOG_TRACE(L"file end");
      break;
    }
  }

  return count;
}

int fchmod(int fd, SceKernelMode mode) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int rename(const char* from, const char* to) {
  auto mapped1 = accessFileManager().getMappedPath(from);
  if (!mapped1) {
    return getErr(ErrCode::_EACCES);
  }
  auto mapped2 = accessFileManager().getMappedPath(to);
  if (!mapped2) {
    return getErr(ErrCode::_EACCES);
  }

  std::filesystem::rename(*mapped1, *mapped2);
  return Ok;
}

int mkdir(const char* path, SceKernelMode mode) {
  auto _mapped = accessFileManager().getMappedPath(path);
  if (!_mapped) {
    return getErr(ErrCode::_EACCES);
  }
  auto const mapped = _mapped.value();

  if (!std::filesystem::create_directory(mapped)) {
    return getErr(ErrCode::_EIO);
  }

  return Ok;
}

int rmdir(const char* path) {
  auto mapped = accessFileManager().getMappedPath(path);
  if (!mapped) {
    return getErr(ErrCode::_EACCES);
  }
  std::filesystem::remove_all(*mapped);
  return Ok;
}

int utimes(const char* path, const SceKernelTimeval* times) {
  return Ok;
}

int stat(const char* path, SceKernelStat* sb) {
  LOG_USE_MODULE(filesystem);

  LOG_TRACE(L"KernelStat: %S", path);

  memset(sb, 0, sizeof(SceKernelStat));
  auto _mapped = accessFileManager().getMappedPath(path);
  if (!_mapped) {
    return getErr(ErrCode::_EACCES);
  }
  auto const mapped = _mapped.value();

  if (!std::filesystem::exists(mapped)) {
    return getErr(ErrCode::_ENOENT);
  }

  bool const isDir = std::filesystem::is_directory(mapped);
  sb->mode         = 0000777u | (isDir ? 0040000u : 0100000u);

  if (isDir) {
    sb->size    = 0;
    sb->blksize = 512;
    sb->blocks  = 0;
  } else {
    sb->size    = (int64_t)std::filesystem::file_size(mapped);
    sb->blksize = 512;
    sb->blocks  = (sb->size + 511) / 512;

    auto const lastW = std::filesystem::last_write_time(mapped);

    auto const time = std::chrono::time_point_cast<std::chrono::nanoseconds>(lastW).time_since_epoch().count();

    ns2timespec(&sb->aTime, time);
    ns2timespec(&sb->mTime, time);
  }

  sb->cTime     = sb->aTime;
  sb->birthtime = sb->mTime;

  return Ok;
}

int fstat(int fd, SceKernelStat* sb) {
  auto mapped = accessFileManager().getPath(fd);
  if (mapped.empty()) {
    return getErr(ErrCode::_EACCES);
  }
  return stat(mapped.string().c_str(), sb);
}

int futimes(int fd, const SceKernelTimeval* times) {
  return Ok;
}

int getdirentries(int fd, char* buf, int nbytes, long* basep) {
  if (fd < FILE_DESCRIPTOR_MIN) {
    return getErr(ErrCode::_EPERM);
  }
  if (buf == nullptr) {
    return getErr(ErrCode::_EFAULT);
  }

  auto count = accessFileManager().getDents(fd, buf, nbytes, (int64_t*)basep);
  if (count < 0) {
    return getErr(ErrCode::_EINVAL);
  }

  return count;
}

int getdents(int fd, char* buf, int nbytes) {
  return getdirentries(fd, buf, nbytes, nullptr);
}

size_t preadv(int handle, const SceKernelIovec* iov, int iovcnt, int64_t offset) {
  LOG_USE_MODULE(filesystem);
  LOG_TRACE(L"preadv [%d]", handle);

  if (offset < 0) {
    return getErr(ErrCode::_EINVAL);
  }

  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    LOG_ERR(L"preadv[%d] file==nullptr", handle);
    return getErr(ErrCode::_EBADF);
  }

  file->clearError();
  file->lseek(offset, SceWhence::beg);

  auto const count = readv(handle, iov, iovcnt);
  if (auto err = file->getErr(); err != Ok) {
    return getErr((ErrCode)err);
  }

  return count;
}

size_t pwritev(int handle, const SceKernelIovec* iov, int iovcnt, int64_t offset) {
  LOG_USE_MODULE(filesystem);
  LOG_TRACE(L"pwritev [%d]", handle);

  if (offset < 0) {
    return getErr(ErrCode::_EINVAL);
  }

  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    LOG_ERR(L"pwritev[%d] file==nullptr", handle);
    return getErr(ErrCode::_EBADF);
  }

  file->clearError();
  file->lseek(offset, SceWhence::beg);

  auto const count = writev(handle, iov, iovcnt);
  if (auto err = file->getErr(); err != Ok) {
    return getErr((ErrCode)err);
  }

  return count;
}

size_t pread(int handle, void* buf, size_t nbytes, int64_t offset) {
  LOG_USE_MODULE(filesystem);
  LOG_TRACE(L"pread [%d]", handle);

  if (buf == nullptr) {
    return getErr(ErrCode::_EFAULT);
  }

  if (offset < 0) {
    return getErr(ErrCode::_EINVAL);
  }

  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    LOG_ERR(L"pread[%d] file==nullptr: 0x%08llx:%llu", handle, (uint64_t)buf, nbytes);
    return getErr(ErrCode::_EBADF);
  }

  file->clearError();
  file->lseek(offset, SceWhence::beg);

  auto const count = file->read((char*)buf, nbytes);
  if (auto err = file->getErr(); err != Ok) {
    return getErr((ErrCode)err);
  }

  LOG_TRACE(L"pread[%d]: 0x%08llx:%llu read(%lld) offset:0x%08llx", handle, (uint64_t)buf, nbytes, count, offset);
  return count;
}

size_t pwrite(int handle, const void* buf, size_t nbytes, int64_t offset) {
  LOG_USE_MODULE(filesystem);
  LOG_TRACE(L"pwrite[%d]: 0x%08llx:%llu", handle, (uint64_t)buf, nbytes);
  auto file = accessFileManager().accessFile(handle);
  if (file == nullptr) {
    LOG_ERR(L"write[%d] file==nullptr: 0x%08llx:%llu", handle, (uint64_t)buf, nbytes);
    return getErr(ErrCode::_EBADF);
  }

  file->clearError();
  file->lseek(offset, SceWhence::beg);

  auto const count = file->write((char*)buf, nbytes);
  if (auto err = file->getErr(); err != Ok) {
    return getErr((ErrCode)err);
  }

  return count;
}

int64_t lseek(int handle, int64_t offset, int whence) {
  LOG_USE_MODULE(filesystem);
  LOG_TRACE(L"lseek [%d] 0x%08llx %d", handle, offset, whence);

  if (whence > 2) return getErr(ErrCode::_EINVAL);

  auto file = accessFileManager().accessFile(handle);
  file->clearError();

  auto const pos = file->lseek(offset, (SceWhence)whence);

  if (auto err = file->getErr(); err != Ok) {
    return getErr((ErrCode)err);
  }
  return pos;
}

int truncate(const char* path, int64_t length) {
  auto mapped = accessFileManager().getMappedPath(path);
  if (!mapped) {
    return getErr(ErrCode::_EACCES);
  }
  std::filesystem::resize_file(*mapped, length);
  return Ok;
}

int ftruncate(int fd, int64_t length) {
  auto mapped = accessFileManager().getPath(fd);
  if (mapped.empty()) {
    return getErr(ErrCode::_EACCES);
  }
  return truncate(mapped.string().c_str(), length);
}

int setCompressionAttribute(int fd, int flag) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int lwfsSetAttribute(int fd, int flags) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int lwfsAllocateBlock(int fd, int64_t size) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int lwfsTrimBlock(int fd, int64_t size) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"todo %S", __FUNCTION__);
  return Ok;
}

int64_t lwfsLseek(int fd, int64_t offset, int whence) {
  LOG_USE_MODULE(filesystem);
  LOG_ERR(L"seek [%d]", fd);
  return Ok;
}

size_t lwfsWrite(int fd, const void* buf, size_t nbytes) {
  return Ok;
}
} // namespace filesystem
