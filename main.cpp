#include "core/runtime/asm/helper.h"
#define __APICALL_IMPORT
#include "core/dmem/memoryManager.h"
#include "core/fileManager/fileManager.h"
#include "core/initParams/initParams.h"
#include "core/ipc/events.h"
#include "core/ipc/ipc.h"
#include "core/ipc/readers/EmulatorRunGame.h"
#include "core/kernel/pthread.h"
#include "core/runtime/exports/intern.h"
#include "core/runtime/procParam.h"
#include "core/runtime/runtimeLinker.h"
#include "core/systemContent/systemContent.h"
#include "core/timer/timer.h"
#include "gamereport.h"
#include "modules/internal/videoout/videoout.h"
#undef __APICALL_IMPORT
#include "logging.h"
#include "utility/progloc.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <set>
#include <thread>
#include <windows.h>

LOG_DEFINE_MODULE(MAIN);

void SYSV_ABI exitHandler() {
  LOG_USE_MODULE(MAIN);
  LOG_ERR(L"Program exit");

  accessRuntimeLinker().stopModules();
}

SYSV_ABI void* thread_func(void* entryAddr) {
  accessRuntimeLinker().callInitProgramms();

  auto& entryParams = accessRuntimeLinker().getEntryParams();

  jmpEntry((uint64_t)entryAddr, entryParams.data(), entryParams.size(), (void*)exitHandler);
  return nullptr;
}

bool loadModule(std::string_view strFullpath) {
  LOG_USE_MODULE(MAIN);

  std::filesystem::path fullpath(strFullpath);

  auto ifFile = util::openFile(fullpath);

  if (!ifFile) {
    LOG_CRIT(L"Couldn't open file %s", fullpath.c_str());
    return false;
  }

  std::shared_ptr<IFormat> formatFile = buildParser_Elf64(std::move(fullpath), std::move(ifFile));

  LOG_INFO(L"---> Parse (%s)", formatFile->getFilename().c_str());

  if (!formatFile->init()) {
    LOG_CRIT(L"<--- Parse (%s): Error: init", formatFile->getFilename().c_str());
    return false;
  }

  uint64_t baseSize = 0, baseSizeAligned = 0, allocSize = 0;
  formatFile->getAllocInfo(baseSize, baseSizeAligned, allocSize);

  auto program = accessRuntimeLinker().createProgram(formatFile->getFilepath(), baseSize, baseSizeAligned, allocSize, true);

  if (!formatFile->load2Mem(program.get())) {
    LOG_CRIT(L"<--- Parse (%s): Error:accessRuntimeLinker", formatFile->getFilename().c_str());
    return false;
  }

  (void)accessRuntimeLinker().addProgram(std::move(program), formatFile);

  LOG_INFO(L"<--- Parse (%s): %S", formatFile->getFilename().c_str(), util::getBoolStr(true));
  return true;
}

void runGame(ScePthread_obj* thread, const std::string_view main, const std::string_view mainRoot, const std::string_view updateRoot) {
  LOG_USE_MODULE(MAIN);

  std::filesystem::path const dirRoot(!mainRoot.empty() ? mainRoot.data() : std::filesystem::path(main).parent_path());

  std::string mainProgPath(main);

  auto& systemContent = accessSystemContent();
  auto& fileManager   = accessFileManager();

  // read param.sfo
  if (!updateRoot.empty()) systemContent.init(std::filesystem::path(updateRoot) / "sce_sys");
  systemContent.init(dirRoot / "sce_sys"); // ignored if updateRoot init was successfull
                                           // -

  std::filesystem::path const dirSaveFiles = [&] {
    auto root  = util::getProgramLoc();
    auto value = systemContent.getString("TITLE_ID");
    if (!value) {
      return root / std::filesystem::path("GAMEFILES") / dirRoot;
    }
    return root / std::filesystem::path("GAMEFILES") / *value;
  }();

  fileManager.setGameFilesDir(dirSaveFiles);
  fileManager.addMountPoint(MOUNT_POINT_APP, dirRoot, MountType::App);

  // Special case: update (app1)
  if (!updateRoot.empty()) {
    fileManager.addMountPoint(MOUNT_POINT_UPDATE, updateRoot, MountType::Update);
    fileManager.enableUpdateSearch();

    // Change eboot.bin to update folder
    auto u_eboot = (std::filesystem::path(updateRoot) / std::filesystem::path(main).filename()).string();
    if (std::filesystem::exists(u_eboot)) mainProgPath = u_eboot;
  }
  // - special case

  fileManager.addMountPoint(MOUNT_POINT_TEMP, dirSaveFiles / MOUNT_POINT_TEMP, MountType::Temp);
  fileManager.addMountPoint(MOUNT_POINT_DATA, dirSaveFiles / MOUNT_POINT_DATA, MountType::Data);
  // --- Setup

  // ### Load modules

  intern::init();
  loadModule(mainProgPath); // load mainprog

  std::set<std::string> moduleSet;

  // Load libs from override
  if (std::filesystem::exists(std::filesystem::path(".override"))) {
    for (auto file: std::filesystem::directory_iterator(std::filesystem::path(".override"))) {
      if ((file.path().extension() == ".prx" || file.path().extension() == ".sprx") && moduleSet.find(file.path().filename().string()) == moduleSet.end()) {
        moduleSet.emplace(file.path().filename().string());

        loadModule(file.path().string());
      }
    }
  }

  // Load libs from update
  if (!updateRoot.empty() && std::filesystem::exists(std::filesystem::path("updateRoot") / "sce_module")) {
    for (auto file: std::filesystem::directory_iterator(std::filesystem::path(updateRoot) / "sce_module")) {
      if (file.path().extension() == ".prx" && moduleSet.find(file.path().filename().string()) == moduleSet.end()) {
        moduleSet.emplace(file.path().filename().string());
        loadModule(file.path().string());
      }
    }
  }

  // Load libs from base
  if (std::filesystem::exists(dirRoot / "sce_module")) {
    for (auto file: std::filesystem::directory_iterator(dirRoot / "sce_module")) {
      if (file.path().extension() == ".prx" && moduleSet.find(file.path().filename().string()) == moduleSet.end()) {
        moduleSet.emplace(file.path().filename().string());
        loadModule(file.path().string());
      }
    }
  }
  // --- modules
  auto& rt = accessRuntimeLinker();

  auto  mainProg  = rt.accessMainProg();
  auto* procParam = (ProcParam*)rt.accessMainProg()->procParamVaddr;

  // Set flexiblememory size if available
  if (procParam->header.size > (8 + offsetof(ProcParam, PSceLibcParam))) {
    auto memparam = (SceKernelMemParam*)((uint64_t)procParam->_sceKernelMemParam + mainProg->baseVaddr);
    if (procParam->_sceKernelMemParam != 0 && memparam->sceKernelFlexibleMemorySize != nullptr) {
      accessMemoryManager()->flexibleMemory()->setTotalSize(*memparam->sceKernelFlexibleMemorySize);
    }
  }

  accessTimer().init();

  auto const entryAddr = rt.execute();

  LOG_INFO(L"Start| entry:0x%08llx", entryAddr);

  intern::post();

  ScePthreadAttr attr;
  pthread::attrInit(&attr);

  // set thread stack size if available
  if (procParam->header.size > (8 + offsetof(ProcParam, sceUserMainThreadStackSize))) {
    if (procParam->sceUserMainThreadStackSize != 0 && *procParam->sceUserMainThreadStackSize != 0)
      pthread::attrSetstacksize(&attr, *procParam->sceUserMainThreadStackSize);
  }

  pthread::create(thread, &attr, thread_func, (void*)entryAddr, "main");
}

int main(int argc, char** argv) {
  ::setvbuf(stdout, nullptr, _IONBF, 0);
  ::setvbuf(stderr, nullptr, _IONBF, 0);
  LOG_USE_MODULE(MAIN);

  printf("Renderer version: %s\n", PSOFF_RENDER_VERSION);

  /**
   * Initialize GameReport before anything else gets loaded.
   * TODO: Catch exceptions and pass them to it if possible.
   */
  (void)accessGameReport();

  auto initParams = accessInitParams();

  // ### Preinit
  if (!initParams->init(argc, argv)) {
    return -1;
  }
  util::setThreadName("MainThread");

  // Wait on debugger if requested
  if (initParams->isDebug()) {
    while (!::IsDebuggerPresent()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  // -

  LOG_INFO(L"Started");

  // Connect to specified pipe
  do {
    auto pipe = initParams->getPipeName();
    if (pipe.empty()) break;

    if (accessIPC().init(pipe.c_str())) {
      LOG_INFO(L"Connecting to pipe %S...", pipe.c_str());
      accessIPC().addHandler([](uint32_t id, uint32_t size, const char* data) {
        switch ((IpcEvent)id) {
          case IpcEvent::EMU_RUN_GAME: {
            IPCEmulatorRunGameRead rg(data, size);

            ScePthread_obj thread;
            runGame(&thread, rg.getExecutable(), rg.getRoot(), rg.getUpdate());
            pthread::detach(thread);
          } break;

          default: break;
        }
      });
      accessIPC().runReadLoop();
      LOG_CRIT(L"IPC ReadLoop is closed!");
    } else {
      LOG_CRIT(L"Pipe conection failed!");
    }
  } while (0); // -

  // --- preinit

  // ### Setup
  auto const filepath = initParams->getApplicationPath();
  if (filepath.empty()) {
    LOG_CRIT(L"--file argument is missing");
    __Log::flush();
    return 1;
  }
  auto const fileRoot   = initParams->getApplicationRoot();
  auto const updateRoot = initParams->getUpdateRoot();

  ScePthread_obj thread;
  runGame(&thread, filepath, fileRoot, updateRoot);
  void* ret = 0;
  pthread::join(thread, &ret);

  __Log::flush();
  return 0;
}
