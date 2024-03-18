#pragma once

#include "../ifile.h"

#include <filesystem>
#include <memory>

class TypeDev: public IFile {
  public:
  TypeDev(): IFile(FileType::Device) {}

  virtual ~TypeDev() {}

  // ### Interface
  virtual size_t  read(void* buf, size_t nbytes)          = 0;
  virtual size_t  write(void* buf, size_t nbytes)         = 0;
  virtual int64_t lseek(int64_t offset, SceWhence whence) = 0; // todo: Is there any seekable device?
  virtual bool    isError()                               = 0;
  virtual void    clearError()                            = 0;
  void            sync() final;
};

std::unique_ptr<TypeDev> createType_dev(std::filesystem::path path, std::ios_base::openmode mode);
