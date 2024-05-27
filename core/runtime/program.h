#pragma once

#include "runtimeExport.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

struct ThreadLocalStorage {
  uint32_t  index;
  uintptr_t vaddrImage = 0;
  uint64_t  sizeImage  = 0;
  uint64_t  offset     = 0;

  uint32_t alignment = 0;
};

struct Program;
class IFormat;

struct RelocateHandlerPayload {
  Program* prog;
  IFormat* format;
};

struct Program {
  RelocateHandlerPayload relocatePayload;

  uint64_t baseVaddr    = 0;
  uint64_t entryOffAddr = 0;

  bool started = false;

  bool     failGlobalUnresolved = true;
  uint64_t procParamVaddr       = 0;

  ThreadLocalStorage tls;

  SceKernelModuleInfoEx moduleInfoEx;

  uint64_t pltVaddr   = 0;
  uint32_t pltNum     = 0;
  bool     isMainProg = false;

  std::filesystem::path const filename;
  std::filesystem::path const path;

  int32_t        id;
  uint64_t const baseSize;
  uint64_t const baseSizeAligned;
  uint64_t const desiredBaseAddr;
  uint64_t const allocSize;

  bool const useStaticTLS;

  std::vector<uint8_t> trampolines_fs;

  std::vector<CxaDestructor> cxa;

  Program(std::filesystem::path path_, uint64_t baseSize_, uint64_t baseSizeAligned_, uint64_t desiredBaseAddr_, uint64_t allocSize_, bool useStaticTLS_)
      : filename(path_.filename()),
        path(path_),
        id(-1),
        baseSize(baseSize_),
        baseSizeAligned(baseSizeAligned_),
        desiredBaseAddr(desiredBaseAddr_),
        allocSize(allocSize_),
        useStaticTLS(useStaticTLS_) {}

  ~Program() {}
};
