#pragma once

#include "../ifile.h"

#include <memory>

class TypeRandom: public IFile {
  public:
  TypeRandom(): IFile(FileType::Device) {}

  virtual ~TypeRandom() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  void    sync() final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<TypeRandom> createType_random();
