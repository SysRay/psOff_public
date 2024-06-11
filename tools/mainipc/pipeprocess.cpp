#include "pipeprocess.h"

#include "ipcpacket.h"

#include <format>
#include <thread>

#define BUFSIZE 512

std::mutex procMutex;

std::vector<std::unique_ptr<PipeProcess>> processes = {};

PipeProcess* CreatePipedProcess(const char* procpath, const char* addarg, const char* pipeid) {
  std::string pipePath = std::format("psoff-{}", pipeid);
  std::string procArgs = std::format("{} --pipe={} {}", procpath, pipePath, addarg);

  std::unique_lock lock(procMutex);
  printf("Spawning \"%s\" pipe...\n", pipePath.c_str());
  auto pp = processes.emplace_back(std::make_unique<PipeProcess>(pipePath.c_str())).get();
  printf("Pipe spawned! Now the process...\n");

  STARTUPINFOA sti = {
      .cb = sizeof(STARTUPINFOA),
  };

  PROCESS_INFORMATION pi;

  if (CreateProcessA(procpath, (char*)procArgs.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &sti, &pi)) {
    pp->procId = pi.dwProcessId;

    std::thread reader([pp]() {
      while (!pp->closed) {
        std::unique_lock lock(pp->wrMutex);

        IPCHeader phdr;

        DWORD rd = 0;

        if (pp->read(&phdr, sizeof(phdr)) > 0) {
          pp->reader(pp, &phdr);
        }
      }

      pp->readfinish = true;
    });

    std::thread writer([pp]() {
      while (!pp->closed) {
        std::unique_lock lock(pp->wrMutex);

        if (pp->vWriteData.size() == 0) {
          Sleep(10);
          continue;
        }

        size_t   sent = 0;
        uint32_t prio = 0;

        pp->mq.send(pp->vWriteData.data(), pp->vWriteData.size(), prio);
        pp->vWriteData.erase(pp->vWriteData.begin(), pp->vWriteData.end());
      }

      pp->writefinish = true;
    });

    reader.detach();
    writer.detach();
    return pp;
  } else {
    printf("Oh no! Failed to spawn process %s: %lu!\n", procpath, GetLastError());
    pp->closed = true;
  }

  return nullptr;
}
