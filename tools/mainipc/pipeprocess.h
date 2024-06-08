#pragma once

#include <mutex>
#include <vector>
#include <windows.h>

struct PipeProcess {
  bool              closed;
  HANDLE            hPipe;
  DWORD             procId;
  std::vector<char> vWriteData;

  std::mutex wrMutex;

  PipeProcess(HANDLE pipe): hPipe(pipe), vWriteData() {}
};

PipeProcess* CreatePipedProcess(const char* procpath, const char* addarg, const char* pipeid);
