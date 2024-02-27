#pragma once
#include "codes.h"

struct SceCoredumpStopInfoCpu {
  void* thread;
  int   reason_code;
};

struct SceCoredumpStopInfoGpu {
  uint64_t timestamp;
};

struct SceCoredumpThreadContextInfo {
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t r8;
  uint64_t r9;
  uint64_t rax;
  uint64_t rbx;
  uint64_t rbp;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rip;
  uint64_t rflags;
  uint64_t rsp;
};