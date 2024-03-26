#pragma once

#include "utility/utility.h"

#include <stdint.h>

enum class FileType {
  File,
  Device,
};

enum class SceWhence : int {
  beg,
  cur,
  end,
};

class IFile {
  CLASS_NO_COPY(IFile);
  CLASS_NO_MOVE(IFile);
  FileType const m_type;

  protected:
  int32_t m_errCode = 0;

  IFile(FileType type): m_type(type) {}

  public:
  virtual ~IFile() = default;

  auto getType() const { return m_type; }

  auto getErr() const { return m_errCode; }

  void clearError() { m_errCode = 0; }

  /**
   * @brief read n bytes from file to (uint8_t)buf[nbytes]
   *
   * @param buf
   * @param nbytes
   * @return size_t number of bytes read, 0:empty
   */
  virtual size_t read(void* buf, size_t nbytes) = 0;

  /**
   * @brief write n bytes from (uint8_t)buf[nbytes] to file
   *
   * @param buf
   * @param nbytes
   * @return size_t number of bytes written
   */
  virtual size_t write(void* buf, size_t nbytes) = 0;

  virtual void sync() = 0;

  /**
   * @brief Set the read/write position
   *
   * @param offset
   * @param whence
   * @return int64_t
   */
  virtual int64_t lseek(int64_t offset, SceWhence whence) = 0;

  virtual void* getNative() = 0;
};
