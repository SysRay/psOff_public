#include "type_file.h"

#include <fstream>

class TypeFile: public IFile {
  std::fstream m_file;

  std::ios_base::openmode const m_mode;

  public:
  TypeFile(std::filesystem::path path, std::ios_base::openmode mode): IFile(FileType::File), m_mode(mode) { m_file = std::fstream(path, mode); }

  virtual ~TypeFile() { sync(); }

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  void    sync() final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<IFile> createType_file(std::filesystem::path path, std::ios_base::openmode mode) {
  return std::make_unique<TypeFile>(path, mode);
}

size_t TypeFile::read(void* buf, size_t nbytes) {
  m_file.read((char*)buf, nbytes);
  return m_file.gcount();
}

size_t TypeFile::write(void* buf, size_t nbytes) {
  auto const start = m_file.tellp(); // current pos
  m_file.write((char*)buf, nbytes);
  return m_file.tellp() - start;
}

void TypeFile::sync() {
  m_file.sync();
}

int64_t TypeFile::lseek(int64_t offset, SceWhence whence) {
  static int _whence[] = {
      std::ios::beg,
      std::ios::cur,
      std::ios::end,
  };

  int64_t ret = 0;
  if ((m_mode & std::ios::in) > 0) {
    ret = m_file.tellg();
    m_file.seekg(offset, _whence[(int)whence]);
  }
  if ((m_mode & std::ios::out) > 0) {
    ret = m_file.tellp();
    m_file.seekp(offset, _whence[(int)whence]);
  }

  return ret;
}

bool TypeFile::isError() {
  return !m_file;
}

void TypeFile::clearError() {
  m_file.clear();
}