#define __APICALL_EXTERN
#include "videoout.h"

#include "intern.h"
#undef __APICALL_EXTERN

#include "config_emu.h"
#include "core/imports/exports/graphics.h"
#include "core/imports/imports_func.h"
#include "core/initParams/initParams.h"
#include "core/kernel/eventqueue.h"
#include "core/systemContent/systemContent.h"
#include "core/timer/timer.h"
#include "gamereport.h"
#include "imageHandler.h"
#include "logging.h"
#include "modules/libSceVideoOut/codes.h"
#include "modules/libSceVideoOut/types.h"
#include "modules_include/common.h"
#include "vulkan/vulkanHelper.h"

#include <queue>

#include <SDL.h>
#include <SDL_syswm.h>
#include <algorithm>
#include <array>
#include <assert.h>
#include <format>
#include <functional>
#include <list>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <mutex>
#include <optick.h>
#include <thread>

LOG_DEFINE_MODULE(VideoOut);
using namespace Kernel;

namespace {
size_t constexpr WindowsMAX = 2;

std::pair<uint32_t, uint32_t> getDisplayBufferSize(uint32_t width, uint32_t height, uint32_t pitch, bool tile, bool neo) {
  if (pitch == 3840) {
    if (width == 3840 && height == 2160 && tile && !neo) {
      return {33423360, 32768};
    }
    if (width == 3840 && height == 2160 && tile && neo) {
      return {33423360, 65536};
    }
    if (width == 3840 && height == 2160 && !tile && !neo) {
      return {33177600, 256};
    }
    if (width == 3840 && height == 2160 && !tile && neo) {
      return {33177600, 256};
    }
  }

  if (pitch == 1920) {
    if (width == 1920 && height == 1080 && tile && !neo) {
      return {8355840, 32768};
    }
    if (width == 1920 && height == 1080 && tile && neo) {
      return {8847360, 65536};
    }
    if (width == 1920 && height == 1080 && !tile && !neo) {
      return {8294400, 256};
    }
    if (width == 1920 && height == 1080 && !tile && neo) {
      return {8294400, 256};
    }
  }

  if (pitch == 1280) {
    if (width == 1280 && height == 720 && tile && !neo) {
      return {3932160, 32768};
    }
    if (width == 1280 && height == 720 && tile && neo) {
      return {3932160, 65536};
    }
    if (width == 1280 && height == 720 && !tile && !neo) {
      return {3686400, 256};
    }
    if (width == 1280 && height == 720 && !tile && neo) {
      return {3686400, 256};
    }
  }
  return {0, 0};
}

struct VideoOutConfig {
  SceVideoOutFlipStatus       flipStatus;
  SceVideoOutVblankStatus     vblankStatus;
  SceVideoOutResolutionStatus resolution;

  std::array<vulkan::SwapchainData, 16> bufferSets;

  std::array<int32_t, 16> buffers; // index to bufferSets

  uint8_t buffersSetsCount = 0;

  double fps = 0.0;

  VideoOutConfig() { std::fill(buffers.begin(), buffers.end(), -1); }
};

enum class FlipRate {
  _60Hz,
  _30Hz,
  _20Hz,
};

struct Context: public ImageHandlerCB {
  int      userId   = -1;
  FlipRate fliprate = FlipRate::_60Hz;

  VideoOutConfig config;
  SDL_Window*    window;
  VkSurfaceKHR   surface = nullptr;

  std::list<EventQueue::IKernelEqueue_t> eventFlip;
  std::list<EventQueue::IKernelEqueue_t> eventVblank;

  // ## interface
  VkExtent2D getWindowSize() final {
    SDL_GetWindowSize(window, (int*)(&config.resolution.paneWidth), (int*)(&config.resolution.paneHeight));
    return VkExtent2D {config.resolution.paneWidth, config.resolution.paneHeight};
  }
};

enum class MessageType { open, close, flip };

struct Message {
  MessageType type;
  int         handle = -1;
  bool*       done   = nullptr;

  int      index    = 0;
  uint32_t setIndex = 0;

  ImageData imageData = {};
};

std::string getTitle(int handle, uint64_t frame, size_t fps, FlipRate maxFPS) {
  static auto title = [] {
    auto title = accessSystemContent().getString("TITLE");
    if (title) return title.value().data();
    return "psOFF";
  }();

  return std::format("{}({}): frame={} fps={}(locked:{})", title, handle, frame, fps, magic_enum::enum_name(maxFPS).data());
}

} // namespace

class VideoOut: public IVideoOut, private IEventsGraphics {
  std::array<Context, WindowsMAX> m_windows;

  uint32_t m_widthTotal = 1920, m_heightTotal = 1080;

  mutable std::mutex m_mutexInt;
  vulkan::VulkanObj* m_vulkanObj = nullptr;

  std::unique_ptr<IGraphics>     m_graphics;
  std::unique_ptr<IImageHandler> m_imageHandler;

  std::thread             m_threadSDL2;
  std::condition_variable m_condSDL2;
  std::condition_variable m_condDone;

  bool m_stop     = false;
  bool m_useVsync = true;

  std::queue<Message> m_messages;

  uint64_t m_vblankTime = (uint64_t)(1e6 / 59.0); // in us

  void vblankEnd(int handle, uint64_t curTime, uint64_t curProcTime);

  // Callback Graphics
  void eventDoFlip(int handle, int index, int64_t flipArg, VkSemaphore waitSema, size_t waitValue) final {
    OPTICK_EVENT();
    LOG_USE_MODULE(VideoOut);

    std::unique_lock lock(m_mutexInt);

    m_graphics->submited();

    auto&          window   = m_windows[handle - 1];
    uint32_t const setIndex = window.config.buffers[index];

    auto& swapchain         = window.config.bufferSets[setIndex];
    auto& displayBufferMeta = swapchain.buffers[index];

    auto imageData = m_imageHandler->getImage_blocking();

    auto& flipStatus = window.config.flipStatus;
    ++flipStatus.gcQueueNum;
    flipStatus.flipArg = flipArg;

    auto&      timer   = accessTimer();
    auto const curTime = (uint64_t)(1e9 * timer.getTimeS());

    flipStatus.submitTsc = curTime;

    if (!imageData) {
      LOG_ERR(L"<- submitFlip(%d):%u %d error", handle, setIndex, index);
      m_graphics->submitDone();
      doFlip(window, handle);
      return;
    }

    LOG_DEBUG(L"-> eventDoFlip(%d):%u %d", handle, setIndex, index);
    transferDisplay(*imageData, displayBufferMeta, waitSema, waitValue);

    // window.config.flipStatus.currentBuffer = index; // set after flip, before vblank

    m_messages.push({MessageType::flip, handle - 1, nullptr, index, setIndex, *imageData});

    LOG_DEBUG(L"<- eventDoFlip(%d):%u %d", handle, setIndex, index);

    lock.unlock();
    if (!m_useVsync) m_condSDL2.notify_one();
  }

  vulkan::QueueInfo* getQueue(vulkan::QueueType type) final;
  // -

  void transferDisplay(ImageData const& imageData, vulkan::SwapchainData::DisplayBuffers& displayBufferMeta, VkSemaphore waitSema, size_t waitValue);

  std::thread createSDLThread();

  void doFlip(Context& ctx, int handle);

  public:
  VideoOut() = default;

  virtual ~VideoOut();

  /**
   * @brief Preinit handle 1(index 0) with main window
   * Needed for early vulkan init and with it all the managers
   */
  void init() final;
  int  open(int userId) final;
  void close(int handle) final;

  void setFliprate(int handle, int rate) final {
    LOG_USE_MODULE(VideoOut);
    LOG_INFO(L"Fliprate:%d", rate);
    std::unique_lock const lock(m_mutexInt);
    m_windows[handle - 1].fliprate = (FlipRate)rate;

    switch ((FlipRate)rate) {
      case FlipRate::_60Hz: m_vblankTime = (uint64_t)(1e6 / 59.0); break;
      case FlipRate::_30Hz: m_vblankTime = (uint64_t)(1e6 / 29.0); break;
      case FlipRate::_20Hz: m_vblankTime = (uint64_t)(1e6 / 19.0); break;
    }
  }

  void getSafeAreaRatio(float* area) final {
    auto ext = m_imageHandler.get()->getExtent();
    if (area != nullptr) *area = ext.width / (float)ext.height;
  }

  vulkan::DeviceInfo* getDeviceInfo() final { return &m_vulkanObj->deviceInfo; }

  int  addEvent(int handle, EventQueue::KernelEqueueEvent const& event, Kernel::EventQueue::IKernelEqueue_t eq) final;
  void removeEvent(int handle, Kernel::EventQueue::IKernelEqueue_t eq, int const ident) final;
  void submitFlip(int handle, int index, int64_t flipArg) final; // -> Renderer

  void getFlipStatus(int handle, void* status) final;
  void getVBlankStatus(int handle, void* status) final;

  void getResolution(int handle, void* status) final;

  void getBufferAttribute(void* attribute, uint32_t pixel_format, int32_t tiling_mode, int32_t aspect_ratio, uint32_t width, uint32_t height,
                          uint32_t pitch_in_pixel) final;

  int registerBuffers(int handle, int startIndex, void* const* addresses, int numBuffer, const void* attribute) final;

  int getPendingFlips(int handle) final {
    std::unique_lock const lock(m_mutexInt);

    auto& flipStatus = m_windows[handle - 1].config.flipStatus;
    return flipStatus.gcQueueNum;
  }

  IGraphics* getGraphics() final {
    assert(m_graphics);
    return m_graphics.get();
  };

  std::unique_lock<std::mutex> getSDLLock() const final { return std::unique_lock(m_mutexInt); }

  // ### Gpu memory forwards
  bool isGPULocal(uint64_t vaddr) final { return m_graphics->isGPULocal(vaddr); }

  bool notify_allocHeap(uint64_t vaddr, uint64_t size, int memoryProtection) final { return m_graphics->notify_allocHeap(vaddr, size, memoryProtection); }

  // --- Gpu memory forwards
};

IVideoOut& accessVideoOut() {
  static VideoOut inst;
  return inst;
}

VideoOut::~VideoOut() {
  printf("saving config files\n");
  accessConfig()->save((uint32_t)ConfigModFlag::LOGGING | (uint32_t)ConfigModFlag::GRAPHICS | (uint32_t)ConfigModFlag::AUDIO |
                       (uint32_t)ConfigModFlag::CONTROLS | (uint32_t)ConfigModFlag::GENERAL);
  // Logging doesn't work here, even with flush
  printf("shutting down VideoOut\n");
  m_stop = true;
  m_condSDL2.notify_one();

  // printf("VideoOut| waiting on gpu idle\n");
  m_imageHandler->stop();
  m_graphics->stop();
  vkQueueWaitIdle(m_imageHandler->getQueue()->queue);

  // shutdown graphics first (uses vulkan)
  m_graphics->deinit();
  m_graphics.reset();

  // printf("VideoOut| Destroy surface\n");
  // for (auto& window: m_windows) {
  //   if (window.userId < 0) continue;
  //   if (window.surface != nullptr) vkDestroySurfaceKHR(m_vulkanObj->deviceInfo.instance, window.surface, nullptr);
  // }

  m_imageHandler->deinit();
  m_imageHandler.reset();

  printf("VideoOut| Destroy vulkan\n");

  deinitVulkan(m_vulkanObj);

  printf("shutdown VideoOut done\n");
}

void VideoOut::init() {
  LOG_USE_MODULE(VideoOut);
  LOG_DEBUG(L"createSDLThread()");
  m_threadSDL2 = createSDLThread();

  std::unique_lock lock(m_mutexInt);
  static bool      done = false;
  m_messages.push(Message {MessageType::open, 0, &done});
  lock.unlock();
  m_condSDL2.notify_one();

  lock.lock();
  m_condDone.wait(lock, [=] { return done; });
}

int VideoOut::open(int userId) {
  OPTICK_THREAD("libVideOut");

  LOG_USE_MODULE(VideoOut);
  std::unique_lock lock(m_mutexInt);

  // Get Free handle
  int const windowIndex = [&] {
    for (int n = 0; n < m_windows.size(); ++n) {
      if (m_windows[n].userId < 0) {
        m_windows[n].userId              = userId;
        m_windows[n].config.flipStatus   = {};
        m_windows[n].config.vblankStatus = {};
        return n;
      }
    }
    return -1;
    // -
  }();
  // -

  if (windowIndex == 0) return 1; // Special case windows[0] is mainWindow

  // Create new window
  static bool done = false;
  m_messages.push(Message {MessageType::open, windowIndex, &done});
  lock.unlock();
  m_condSDL2.notify_one();

  lock.lock();
  m_condDone.wait(lock, [=] { return done; });
  return 1 + windowIndex; // handle starts with 1
}

void VideoOut::close(int handle) {
  LOG_USE_MODULE(VideoOut);
  LOG_INFO(L"<-- VideoOut Close(%d)", handle);

  std::unique_lock lock(m_mutexInt);

  auto& window  = m_windows[handle - 1];
  window.userId = -1;
  for (auto& item: window.eventFlip) {
    if (item != nullptr) {
      (void)item->deleteEvent(VIDEO_OUT_EVENT_FLIP, EventQueue::KERNEL_EVFILT_VIDEO_OUT);
    }
  }
  for (auto& item: window.eventVblank) {
    if (item != nullptr) {
      (void)item->deleteEvent(VIDEO_OUT_EVENT_VBLANK, EventQueue::KERNEL_EVFILT_VIDEO_OUT);
    }
  }

  window.eventFlip.clear();
  window.eventVblank.clear();

  static bool done = false;
  m_messages.push(Message {MessageType::close, handle, &done});
  lock.unlock();
  m_condSDL2.notify_one();

  lock.lock();
  m_condDone.wait(lock, [=] { return done; });
}

int VideoOut::addEvent(int handle, EventQueue::KernelEqueueEvent const& event, EventQueue::IKernelEqueue_t eq) {
  LOG_USE_MODULE(VideoOut);

  if (eq == nullptr) return getErr(ErrCode::_EINVAL);

  int result = eq->addEvent(event);
  switch (event.event.ident) {
    case VIDEO_OUT_EVENT_FLIP: {
      LOG_DEBUG(L"+VIDEO_OUT_EVENT_FLIP(%d)", handle);
      std::unique_lock const lock(m_mutexInt);
      m_windows[handle - 1].eventFlip.push_back(eq);
    } break;
    case VIDEO_OUT_EVENT_VBLANK: {
      LOG_DEBUG(L"+VIDEO_OUT_EVENT_VBLANK(%d)", handle);
      std::unique_lock const lock(m_mutexInt);
      m_windows[handle - 1].eventVblank.push_back(eq);
    } break;
    default: LOG_ERR(L"undefinded:%d", event.event.ident); result = -1;
  }
  return result;
}

void VideoOut::removeEvent(int handle, Kernel::EventQueue::IKernelEqueue_t eq, int const ident) {
  LOG_USE_MODULE(VideoOut);

  switch (ident) {
    case VIDEO_OUT_EVENT_FLIP: {
      LOG_DEBUG(L"-VIDEO_OUT_EVENT_FLIP(%d)", handle);
      std::unique_lock const lock(m_mutexInt);
      m_windows[handle - 1].eventFlip.remove(eq);
    } break;
    case VIDEO_OUT_EVENT_VBLANK: {
      LOG_DEBUG(L"-VIDEO_OUT_EVENT_VBLANK(%d)", handle);
      std::unique_lock const lock(m_mutexInt);
      m_windows[handle - 1].eventVblank.remove(eq);
    } break;
    default: LOG_CRIT(L"undefinded:%d", ident);
  }
}

void VideoOut::transferDisplay(ImageData const& imageData, vulkan::SwapchainData::DisplayBuffers& displayBufferMeta, VkSemaphore waitSema, size_t waitValue) {

  vulkan::transfer2Display(&displayBufferMeta, imageData, m_graphics.get());
  vulkan::submitDisplayTransfer(&displayBufferMeta, imageData, m_imageHandler->getQueue(), waitSema, waitValue);
}

void VideoOut::submitFlip(int handle, int index, int64_t flipArg) {
  OPTICK_EVENT();
  LOG_USE_MODULE(VideoOut);

  std::unique_lock lock(m_mutexInt);

  m_graphics->submited(); // increase internal counter (wait for flip)

  auto&          window   = m_windows[handle - 1];
  uint32_t const setIndex = window.config.buffers[index];

  auto& swapchain         = window.config.bufferSets[setIndex];
  auto& displayBufferMeta = swapchain.buffers[index];

  LOG_DEBUG(L"-> submitFlip(%d):%u %d", handle, setIndex, index);

  auto imageData = m_imageHandler->getImage_blocking();

  auto& flipStatus = window.config.flipStatus;
  ++flipStatus.gcQueueNum;
  flipStatus.flipArg = flipArg;

  auto&      timer   = accessTimer();
  auto const curTime = (uint64_t)(1e9 * timer.getTimeS());

  flipStatus.submitTsc = curTime;
  if (!imageData) {
    LOG_ERR(L"<- submitFlip(%d):%u %d error", handle, setIndex, index);
    m_graphics->submitDone();
    doFlip(window, handle);
    return;
  }

  transferDisplay(*imageData, displayBufferMeta, nullptr, 0);

  // window.config.flipStatus.currentBuffer = index; // set after flip, before vblank

  m_messages.push({MessageType::flip, handle - 1, nullptr, index, setIndex, *imageData});

  LOG_DEBUG(L"<- submitFlip(%d):%u %d", handle, setIndex, index);

  lock.unlock();
  if (!m_useVsync) m_condSDL2.notify_one();
}

void VideoOut::doFlip(Context& ctx, int handle) {
  auto& flipStatus = ctx.config.flipStatus;

  auto&      timer      = accessTimer();
  auto const curTime    = (uint64_t)(1e6 * timer.getTimeS());
  auto const procTime   = timer.queryPerformance();
  auto       elapsed_us = curTime - flipStatus.processTime;

  flipStatus.tsc         = procTime;
  flipStatus.processTime = curTime;
  ++flipStatus.count;
  --flipStatus.gcQueueNum;
  vblankEnd(handle, curTime, procTime);

  // Trigger Event Flip
  for (auto& item: ctx.eventFlip) {
    (void)item->triggerEvent(VIDEO_OUT_EVENT_FLIP, EventQueue::KERNEL_EVFILT_VIDEO_OUT, reinterpret_cast<void*>(ctx.config.flipStatus.flipArg));
  }
  // - Flip event
}

void VideoOut::getFlipStatus(int handle, void* status) {
  LOG_USE_MODULE(VideoOut);
  LOG_TRACE(L"%S(%d)", __FUNCTION__, handle);

  std::unique_lock const lock(m_mutexInt);
  *(SceVideoOutFlipStatus*)status = m_windows[handle - 1].config.flipStatus;
}

void VideoOut::getVBlankStatus(int handle, void* status) {
  LOG_USE_MODULE(VideoOut);
  LOG_TRACE(L"%S(%d)", __FUNCTION__, handle);

  std::unique_lock const lock(m_mutexInt);

  auto& vblank = m_windows[handle - 1].config.vblankStatus;

  *(SceVideoOutVblankStatus*)status = vblank;
}

void VideoOut::vblankEnd(int handle, uint64_t curTime, uint64_t curProcTime) {
  OPTICK_EVENT();

  auto& window = m_windows[handle - 1];
  auto& vblank = window.config.vblankStatus;

  // vblank.processTime = 0;

  vblank.tsc         = curProcTime;
  vblank.processTime = curTime;
  ++vblank.count;

  for (auto& item: window.eventVblank) {
    (void)item->triggerEvent(VIDEO_OUT_EVENT_VBLANK, EventQueue::KERNEL_EVFILT_VIDEO_OUT, reinterpret_cast<void*>(window.config.vblankStatus.count));
  }
}

void VideoOut::getResolution(int handle, void* status) {
  LOG_USE_MODULE(VideoOut);
  LOG_TRACE(L"%S(%d)", __FUNCTION__, handle);
  std::unique_lock const lock(m_mutexInt);

  *(SceVideoOutResolutionStatus*)status = m_windows[handle - 1].config.resolution;
}

void VideoOut::getBufferAttribute(void* attribute, uint32_t pixel_format, int32_t tiling_mode, int32_t aspect_ratio, uint32_t width, uint32_t height,
                                  uint32_t pitchInPixel) {
  LOG_USE_MODULE(VideoOut);
  LOG_TRACE(L"%S", __FUNCTION__);

  auto [displayFormat, _] = vulkan::getDisplayFormat(m_vulkanObj);

  // todo: needs gpu memory display image recreate
  // if (m_widthTotal >= 1920 && m_heightTotal >= 1080) {
  //   *(SceVideoOutBufferAttribute*)attribute = SceVideoOutBufferAttribute {
  //       .pixelFormat  = SceVideoOutPixelFormat::PIXEL_FORMAT_A8R8G8B8_SRGB, // todo get vulkan pixel_format?
  //       .tilingMode   = tiling_mode,
  //       .aspectRatio  = aspect_ratio,
  //       .width        = m_widthTotal,
  //       .height       = m_heightTotal,
  //       .pitchInPixel = m_widthTotal,
  //   };
  // } else {
  *(SceVideoOutBufferAttribute*)attribute = SceVideoOutBufferAttribute {
      .pixelFormat  = SceVideoOutPixelFormat::PIXEL_FORMAT_A8R8G8B8_SRGB, // todo get vulkan pixel_format?
      .tilingMode   = tiling_mode,
      .aspectRatio  = aspect_ratio,
      .width        = width,
      .height       = height,
      .pitchInPixel = pitchInPixel,
  };
  //}
}

int VideoOut::registerBuffers(int handle, int startIndex, void* const* addresses, int numBuffer, const void* attribute) {
  LOG_USE_MODULE(VideoOut);
  LOG_TRACE(L"%S(%d)", __FUNCTION__, handle);

  auto&      config   = m_windows[handle - 1].config;
  auto const setIndex = config.buffersSetsCount++;

  [[unlikely]] if (setIndex > 15) {
    LOG_CRIT(L"buffersSetsCount > 15");
    return ::Err::VIDEO_OUT_ERROR_NO_EMPTY_SLOT;
  }

  auto& bufferSet = config.bufferSets[setIndex];
  bufferSet.buffers.resize(numBuffer);

  for (int i = startIndex; i < startIndex + numBuffer; ++i) {
    if (config.buffers[i] >= 0) return ::Err::VIDEO_OUT_ERROR_SLOT_OCCUPIED;
    config.buffers[i] = setIndex;
  }

  auto const* _att = (SceVideoOutBufferAttribute const*)attribute;

  for (size_t n = 0; n < numBuffer; ++n) {
    bufferSet.buffers[n].bufferVaddr           = (uintptr_t)addresses[n];
    auto const [displaySize, displaySizeAlign] = getDisplayBufferSize(_att->width, _att->height, _att->pitchInPixel, _att->tilingMode, false);

    bufferSet.buffers[n].bufferSize  = displaySize;
    bufferSet.buffers[n].bufferAlign = displaySizeAlign;
    LOG_INFO(L"+bufferset[%d] buffer:%d vaddr:0x%08llx-0x%08llx", setIndex, n, (uint64_t)addresses[n], (uint64_t)addresses[n] + displaySizeAlign);

    auto [format, colorSpace] = vulkan::getDisplayFormat(m_vulkanObj);
    if (!m_graphics->registerDisplayBuffer(bufferSet.buffers[n].bufferVaddr, VkExtent2D {.width = _att->width, .height = _att->height}, _att->pitchInPixel,
                                           format))
      return -1;
  }

  return setIndex;
}

vulkan::QueueInfo* VideoOut::getQueue(vulkan::QueueType type) {

  std::unique_lock const lock(m_mutexInt);

  auto& queueInfo = m_vulkanObj->queues.items[vulkan::getIndex(type)];
  auto  bestIt    = queueInfo.begin();

  // Search for least used
  for (auto it = queueInfo.begin()++; it != queueInfo.end(); ++it) {
    if ((*it)->useCount < (*bestIt)->useCount) {
      bestIt = it;
    }
  }
  // -

  ++(*bestIt)->useCount;
  return bestIt->get();
}

void cbWindow_close(SDL_Window* window) {
  LOG_USE_MODULE(VideoOut);

  const SDL_MessageBoxButtonData mbbd[2] {
      {.flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, .buttonid = 0, .text = "Cancel"},
      {.flags = 0, .buttonid = 1, .text = "Quit"},
  };

  const SDL_MessageBoxData mbd {
      .flags       = SDL_MESSAGEBOX_WARNING,
      .window      = window,
      .title       = "Are sure you want to quit?",
      .message     = "All your unsaved progress will be lost!",
      .numbuttons  = 2,
      .buttons     = mbbd,
      .colorScheme = nullptr,
  };

  int buttonId = -1;
  if (SDL_ShowMessageBox(&mbd, &buttonId) != 0) {
    LOG_ERR(L"Failed to generate SDL MessageBox: %S", SDL_GetError());
    return;
  }

  if (buttonId == 1) exit(0); // destructor cleans up.
}

void cbWindow_moveresize(SDL_Window* window) {
  int w, h, x, y;
  auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GRAPHICS);
  SDL_GetWindowSize(window, &w, &h);
  SDL_GetWindowPosition(window, &x, &y);

  (*jData)["display"] = SDL_GetWindowDisplayIndex(window);
  (*jData)["width"] = w, (*jData)["height"] = h;
  (*jData)["xpos"] = x, (*jData)["ypos"] = y;
}

std::thread VideoOut::createSDLThread() {
  return std::thread([this] {
    util::setThreadName("VideoOut");
    OPTICK_THREAD("VideoOut");

    LOG_USE_MODULE(VideoOut);
    LOG_DEBUG(L"Init SDL2 video");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    // SDL polling helper
    auto func_pollSDL = [](auto& window) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
          case SDL_WINDOWEVENT:
            switch (event.window.event) {
              case SDL_WINDOWEVENT_CLOSE: cbWindow_close(window); break;

              case SDL_WINDOWEVENT_RESIZED:
              case SDL_WINDOWEVENT_MOVED: cbWindow_moveresize(window); break;

              default: break;
            }

          case SDL_KEYUP:
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
              cbWindow_close(window);
            } else if (event.key.keysym.scancode == GAMEREPORT_USER_SEND_SCANCODE) {
              auto title    = accessSystemContent().getString("TITLE");
              auto title_id = accessSystemContent().getString("TITLE_ID");
              auto app_ver  = accessSystemContent().getString("APP_VER");

              accessGameReport().ShowReportWindow({
                  .title    = title ? title.value().data() : "Your PS4 Game Name",
                  .title_id = title_id ? title_id.value().data() : "CUSA00000",
                  .app_ver  = app_ver ? app_ver.value().data() : "v0.0",
                  .wnd      = window,

                  .type = IGameReport::Type::USER,
                  .add =
                      {
                          .ex = nullptr,
                      },
              });
            }

          default: break;
        }
      }
    };
    // -

    while (!m_stop) {
      std::unique_lock lock(m_mutexInt);
      m_condSDL2.wait_for(lock, std::chrono::microseconds(m_vblankTime), [this] { return m_stop || !m_messages.empty(); });
      if (m_stop) break;

      // Handle VBlank
      if (m_messages.empty()) {
        using namespace std::chrono;

        func_pollSDL(m_windows[0].window); // check only main for now

        auto&      timer    = accessTimer();
        auto const curTime  = (uint64_t)(1e6 * timer.getTimeS());
        auto const procTime = timer.queryPerformance();
        for (size_t n = 0; n < m_windows.size(); ++n) {

          vblankEnd(1 + n, curTime, procTime);
        }
        continue;
      }
      // -

      auto       item        = m_messages.front();
      auto const handleIndex = item.handle;
      auto&      window      = m_windows[handleIndex];

      switch (item.type) {
        case MessageType::open: {

          auto const title = getTitle(handleIndex, 0, 0, window.fliprate);
          int        win_x = -1, win_y = -1;
          bool       alter = false;
          int        displ = 0;

          Uint32 addFlags = 0;
          { // Handle graphic config
            auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GRAPHICS);

            bool useFullscreen = false;
            if (!getJsonParam(jData, "fullscreen", useFullscreen)) {
              LOG_ERR(L"Config| Couldn't load param:fullscreen from graphics");
            }

            auto readCfgIntParam = [jData](const char* param, auto& value) {
              LOG_USE_MODULE(VideoOut);

              if (!getJsonParam(jData, param, value)) {
                LOG_ERR(L"Config| Couldn't load param:%S from graphics", param);
              }
            };

            if (!useFullscreen) {
              readCfgIntParam("xpos", win_x);
              readCfgIntParam("ypos", win_y);
              readCfgIntParam("display", displ);
              readCfgIntParam("width", m_widthTotal);
              readCfgIntParam("height", m_heightTotal);

              if (m_widthTotal <= 0) m_widthTotal = 1920;
              if (m_heightTotal <= 0) m_heightTotal = 1080;
              if (displ >= SDL_GetNumVideoDisplays()) displ = 0; // User disconnected some displays since the last run?

              if (win_x < 0 || win_y < 0) { // Negative window coords? => Center the window on selected display
                win_x = win_y = SDL_WINDOWPOS_CENTERED_DISPLAY(displ);
                alter         = true;
              } else { // Check the saved window position to be valid according with the current displays configuration
                SDL_Rect db;
                SDL_GetDisplayBounds(displ, &db);
                if ((win_x < db.x) || (win_y < db.y) || (win_x + m_widthTotal > db.x + db.w) || (win_y + m_heightTotal > db.y + db.h)) {
                  win_x = win_y = SDL_WINDOWPOS_CENTERED_DISPLAY(displ);
                  alter         = true;
                }
              }
            }

            if (useFullscreen)
              addFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            else
              addFlags |= SDL_WINDOW_RESIZABLE;
          }

          window.window = SDL_CreateWindow(title.c_str(), win_x, win_y, m_widthTotal, m_heightTotal,
                                           SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | addFlags);

          { // todo: Fix minimize button, now it crashes the emulator.
            SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            SDL_GetWindowWMInfo(window.window, &wmInfo);
            HWND hwnd = wmInfo.info.win.window;
            SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
          }

          SDL_GetWindowSize(window.window, (int*)(&window.config.resolution.paneWidth), (int*)(&window.config.resolution.paneHeight));
          window.config.resolution.fullWidth  = window.config.resolution.paneWidth;
          window.config.resolution.fullHeight = window.config.resolution.paneHeight;
          if (alter) cbWindow_moveresize(window.window); // Trigger config resave if the emulator window is out of user space

          LOG_INFO(L"--> VideoOut Open(%S)| %d:%d", title.c_str(), window.config.resolution.paneWidth, window.config.resolution.paneHeight);
          if (m_vulkanObj == nullptr) {
            m_vulkanObj = vulkan::initVulkan(window.window, window.surface, accessInitParams()->enableValidation());
            auto& info  = m_vulkanObj->deviceInfo;

            m_graphics = createGraphics(*this, info.device, info.physicalDevice, info.instance);

            auto queue = m_vulkanObj->queues.items[getIndex(vulkan::QueueType::present)][0].get(); // todo use getQeueu

            m_useVsync = accessInitParams()->useVSYNC();

            m_imageHandler =
                createImageHandler(info.device, VkExtent2D {window.config.resolution.paneWidth, window.config.resolution.paneHeight}, queue, &window);
            m_imageHandler->init(m_vulkanObj, window.surface);

            *item.done = true;
          } else {
            vulkan::createSurface(m_vulkanObj, window.window, window.surface);
          }

          m_condDone.notify_one();
        } break;
        case MessageType::close: {
          SDL_DestroyWindow(window.window);
          *item.done = true;
          m_condDone.notify_one();
        } break;
        case MessageType::flip: {
          LOG_DEBUG(L"-> flip(%d) set:%u buffer:%u", item.index, item.setIndex, item.imageData.index);
          OPTICK_FRAME("VideoOut");
          auto& flipStatus = window.config.flipStatus;

          {
            OPTICK_EVENT("Present");
            presentImage(item.imageData, m_imageHandler->getSwapchain(), m_imageHandler->getQueue());
            m_imageHandler->notify_done(item.imageData);
          }
          m_graphics->submitDone();

          auto&      timer      = accessTimer();
          auto const curTime    = (uint64_t)(1e6 * timer.getTimeS());
          auto const procTime   = timer.queryPerformance();
          auto       elapsed_us = curTime - flipStatus.processTime;

          doFlip(window, 1 + handleIndex);

          double const fps   = (window.config.fps * 5.0 + (1e6 / (double)elapsed_us)) / 6.0;
          auto         title = getTitle(1 + handleIndex, flipStatus.count, round(fps), window.fliprate);

          window.config.fps = fps;

          SDL_SetWindowTitle(window.window, title.c_str());
          func_pollSDL(window.window);

          LOG_DEBUG(L"<- flip(%d) set:%u buffer:%u", handleIndex, item.setIndex, item.imageData.index);
        } break;
      }
      m_messages.pop();
    }
    SDL_Quit();
  });
}

uint64_t getImageAlignment(VkFormat format, VkExtent3D const& extent) {
  auto device = ((VideoOut&)accessVideoOut()).getDeviceInfo()->device;

  VkImageCreateInfo const imageInfo {
      .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext         = nullptr,
      .flags         = 0,
      .imageType     = VK_IMAGE_TYPE_2D,
      .format        = format,
      .extent        = extent,
      .mipLevels     = 1,
      .arrayLayers   = 1,
      .samples       = VK_SAMPLE_COUNT_1_BIT,
      .tiling        = VK_IMAGE_TILING_OPTIMAL,
      .usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
      .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };

  VkImage              image;
  VkMemoryRequirements reqs;
  vkCreateImage(device, &imageInfo, nullptr, &image);
  vkGetImageMemoryRequirements(device, image, &reqs);
  vkDestroyImage(device, image, nullptr);
  return reqs.alignment;
}
