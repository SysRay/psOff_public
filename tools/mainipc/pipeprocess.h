#pragma once

#include <boost/interprocess/ipc/message_queue.hpp>
#include <functional>
#include <mutex>
#include <vector>
#include <windows.h>

struct IPCHeader;

struct PipeProcess {
  bool                               closed      = false;
  bool                               readfinish  = false;
  bool                               writefinish = false;
  std::recursive_mutex               wrMutex;
  boost::interprocess::message_queue mq;
  DWORD                              procId;
  std::vector<char>                  vWriteData;

  std::function<void(PipeProcess* pproc, IPCHeader* phead)> reader;

  PipeProcess(const char* pipe): mq(boost::interprocess::open_or_create, pipe, 5, 1048576), vWriteData() {}

  size_t read(void* buffer, size_t size) {
    size_t   recv = 0;
    uint32_t prio = 0;
    mq.receive(buffer, size, recv, prio);
    return recv;
  }
};

PipeProcess* CreatePipedProcess(const char* procpath, const char* addarg, const char* pipeid);
