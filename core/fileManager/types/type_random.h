#pragma once

#include "type_dev.h"

#include <memory>

class TypeRandom: public TypeDev {
  public:
  TypeRandom(): TypeDev() {}

  virtual ~TypeRandom() {}

  // ### Interface
  size_t  read(void* buf, size_t nbytes) final;
  size_t  write(void* buf, size_t nbytes) final;
  int64_t lseek(int64_t offset, SceWhence whence) final;
  bool    isError() final;
  void    clearError() final;
};

std::unique_ptr<TypeRandom> createType_random();
