#include "pipeprocess.h"

#include "ipcpacket.h"

#include <format>
#include <thread>

#define BUFSIZE 512

std::mutex procMutex;

std::vector<std::unique_ptr<PipeProcess>> processes = {};

PipeProcess* CreatePipedProcess(const char* procpath, const char* addarg, const char* pipeid) {
  std::string pipePath = std::format("\\\\.\\pipe\\psoff-{}", pipeid);
  std::string procArgs = std::format("{} --pipe={} {}", procpath, pipePath, addarg);

  printf("Spawning \"%s\" pipe...\n", pipePath.c_str());

  HANDLE hPipe = CreateNamedPipeA(pipePath.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFSIZE,
                                  BUFSIZE, 0, nullptr);

  if (hPipe == INVALID_HANDLE_VALUE) {
    printf("Oh no! Failed to create pipe %s :(\n", pipeid);
    return nullptr;
  }

  printf("Pipe spawned! Now the process...\n");

  STARTUPINFOA sti = {
      .cb = sizeof(STARTUPINFOA),
  };

  PROCESS_INFORMATION pi;

  if (CreateProcessA(procpath, (char*)procArgs.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &sti, &pi)) {
    std::unique_lock lock(procMutex);
    processes.emplace_back(std::make_unique<PipeProcess>(hPipe));
    auto& pp = processes.back();

    while (!ConnectNamedPipe(hPipe, NULL)) {
      Sleep(10);
    }
    printf("Got client connection to pipe!\n");

    auto closePipe = [&pi, &pp]() {
      TerminateProcess(pi.hProcess, 1);
      CloseHandle(pi.hProcess);
      pp->closed = true;
    };

    std::thread reader([&closePipe, &pp]() {
      while (!pp->closed) {
        std::unique_lock lock(pp->wrMutex);

        DWORD avail;
        if (!pp->reader || !PeekNamedPipe(pp->hPipe, nullptr, 0, nullptr, &avail, nullptr) || avail == 0 || (GetLastError() != ERROR_BROKEN_PIPE)) {
          Sleep(5);
          continue;
        }

        IPCHeader phdr;

        DWORD rd = 0;

        if (ReadFile(pp->hPipe, &phdr, sizeof(phdr), &rd, nullptr) && rd > 0) {
          pp->reader(pp.get(), &phdr);
        } else {
          switch (GetLastError()) {
            case ERROR_BROKEN_PIPE:
              if (!ConnectNamedPipe(pp->hPipe, nullptr)) {
                printf("Failed to reconnect to the pipe! Killing pipe process...\n");
                closePipe();
                return;
              }
              break;
          }
        }
      }

      pp->readfinish = true;
    });

    std::thread writer([&pp]() {
      while (!pp->closed) {
        std::unique_lock lock(pp->wrMutex);

        if (pp->vWriteData.size() == 0) {
          Sleep(10);
          continue;
        }

        DWORD written;

        if (WriteFile(pp->hPipe, pp->vWriteData.data(), pp->vWriteData.size(), &written, nullptr)) {
          pp->vWriteData.erase(pp->vWriteData.begin(), pp->vWriteData.begin() + written);
        } else {
          printf("Oh no! Failed to write data to pipe: %lu!", GetLastError());
        }
      }

      pp->writefinish = true;
    });

    reader.detach();
    writer.detach();
    return pp.get();
  } else {
    printf("Oh no! Failed to spawn process %s: %lu!\n", procpath, GetLastError());
    CloseHandle(hPipe);
  }

  return nullptr;
}
