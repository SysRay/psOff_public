#define __APICALL_EXTERN
#include "videoout.h"
#undef __APICALL_EXTERN

#include "core/graphics/objects/colorAttachment.h"
#include "core/manager/graphics/managerInit.h"
#include "modules/libSceVideoOut/codes.h"
#include "modules/libSceVideoOut/types.h"
#include "modules_include/common.h"
#include "vulkan/vulkanHelper.h"

#include <queue>

#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <assert.h>
#include <eventqueue_types.h>
#include <format>
#include <graphics.h>
#include <initParams.h>
#include <list>
#include <logging.h>
#include <memory>
#include <mutex>
#include <optick.h>
#include <systemContent.h>
#include <thread>
#include <timer.h>

LOG_DEFINE_MODULE(VideoOut);
using namespace Kernel;

namespace {
size_t constexpr WindowsMAX = 2;

std::string getTitle(int handle, uint64_t frame, size_t fps) {
  static auto title = [] {
    auto title = accessSystemContent().getString("TITLE");
    if (title) return title.value().data();
    return "psOFF";
  }();

  return std::format("{}({}): frame={} fps={}", title, handle, frame, fps);
}

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

  std::array<vulkan::SwapchainData, 16>                   bufferSets;
  std::array<int32_t, 16>                                 buffers; // index to bufferSets
  std::array<std::weak_ptr<Objects::ColorAttachment>, 16> displayBuffers;
  uint8_t                                                 buffersSetsCount = 0;

  double fps = 0.0;

  VideoOutConfig() { std::fill(buffers.begin(), buffers.end(), -1); }
};

struct Context {
  int userId   = -1;
  int fliprate = 3;

  VideoOutConfig config;
  GLFWwindow*    window;
  VkSurfaceKHR   surface;

  std::list<EventQueue::KernelEqueue*> eventFlip;
  std::list<EventQueue::KernelEqueue*> eventVblank;
};

enum class MessageType { open, close, flip };

struct Message {
  MessageType type;
  int         windowIndex = -1;
  bool*       done        = nullptr;
  uint32_t    index       = 0;
};
} // namespace

class VideoOut: public IVideoOut, private IEventsGraphics {
  std::array<Context, WindowsMAX> m_windows;

  uint32_t const m_widthTotal = 1920, m_heightTotal = 1080; // todo: make config

  mutable std::mutex m_mutexInt;
  vulkan::VulkanObj* m_vulkanObj = nullptr;

  std::unique_ptr<IGraphics> m_graphics;
  std::thread                m_threadGlfw;
  std::condition_variable    m_condGlfw;
  std::condition_variable    m_condDone;
  bool                       m_stop = false;
  std::queue<Message>        m_messages;

  void vblankEnd(int handle, uint64_t curTime, uint64_t curProcTime);

  // Callback Graphics
  void eventDoFlip(int handle, int index, int64_t flipArg, VkSemaphore waitSema, size_t waitValue) final {
    OPTICK_EVENT();
    m_graphics->submited();
    transferDisplay(handle, index, waitSema, waitValue);

    std::unique_lock lock(m_mutexInt);
    auto&            window   = m_windows[handle - 1];
    uint32_t const   setIndex = window.config.buffers[index];

    auto& flipStatus = m_windows[index].config.flipStatus;
    ++flipStatus.gcQueueNum;
    flipStatus.flipArg = flipArg;

    auto&      timer   = accessTimer();
    auto const curTime = (uint64_t)(1e9 * timer.getTimeS());

    flipStatus.submitTsc = curTime;
    // window.config.flipStatus.currentBuffer = index; // set after flip, before vblank

    m_messages.push({MessageType::flip, handle - 1, nullptr, setIndex});
    lock.unlock();
    m_condGlfw.notify_one();
  }

  vulkan::DeviceInfo* getDeviceInfo() final { return &m_vulkanObj->deviceInfo; }

  std::pair<VkQueue, uint32_t> getQueue(vulkan::QueueType type) final;
  // -
  void transferDisplay(int handle, int index, VkSemaphore waitSema, size_t waitValue); // -> Renderer

  std::thread createGlfwThread();

  public:
  VideoOut() = default;

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
    m_windows[handle - 1].fliprate = rate;
  }

  int  addEvent(int handle, EventQueue::KernelEqueueEvent const& event, EventQueue::KernelEqueue* eq) final;
  void removeEvent(int handle, Kernel::EventQueue::KernelEqueue* eq, int const ident) final;
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

  std::unique_lock<std::mutex> getGlfwLock() const final { return std::unique_lock(m_mutexInt); }
};

IVideoOut& accessVideoOut() {
  static VideoOut inst;
  return inst;
}

void VideoOut::init() {
  LOG_USE_MODULE(VideoOut);
  LOG_DEBUG(L"createGlfwThread()");
  m_threadGlfw = createGlfwThread();

  std::unique_lock lock(m_mutexInt);
  static bool      done = false;
  m_messages.push(Message {MessageType::open, 0, &done});
  lock.unlock();
  m_condGlfw.notify_one();

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
  m_condGlfw.notify_one();

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
      (void)EventQueue::KernelDeleteEvent(item, VIDEO_OUT_EVENT_FLIP, EventQueue::KERNEL_EVFILT_VIDEO_OUT);
    }
  }
  for (auto& item: window.eventVblank) {
    if (item != nullptr) {
      (void)EventQueue::KernelDeleteEvent(item, VIDEO_OUT_EVENT_VBLANK, EventQueue::KERNEL_EVFILT_VIDEO_OUT);
    }
  }

  window.eventFlip.clear();
  window.eventVblank.clear();

  static bool done = false;
  m_messages.push(Message {MessageType::close, handle, &done});
  lock.unlock();
  m_condGlfw.notify_one();

  lock.lock();
  m_condDone.wait(lock, [=] { return done; });
}

int VideoOut::addEvent(int handle, EventQueue::KernelEqueueEvent const& event, EventQueue::KernelEqueue* eq) {
  LOG_USE_MODULE(VideoOut);

  if (eq == nullptr) return getErr(ErrCode::_EINVAL);

  int result = EventQueue::KernelAddEvent(eq, event);
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

void VideoOut::removeEvent(int handle, Kernel::EventQueue::KernelEqueue* eq, int const ident) {
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

void VideoOut::transferDisplay(int handle, int index, VkSemaphore waitSema, size_t waitValue) {
  LOG_USE_MODULE(VideoOut);

  auto&          window   = m_windows[handle - 1];
  uint32_t const setIndex = window.config.buffers[index];

  auto& swapchain         = window.config.bufferSets[setIndex];
  auto& displayBufferMeta = swapchain.buffers[index];
  if (window.config.displayBuffers[index].expired()) {
    auto image = accessGpuMemory().getDisplayBuffer(displayBufferMeta.bufferVaddr);
    if (image) {
      window.config.displayBuffers[index] = image;
      vulkan::transfer2Display(displayBufferMeta.transferBuffer, m_vulkanObj, swapchain, image->getImage(), image.get(), index);
      vulkan::submitDisplayTransfer(displayBufferMeta.transferBuffer, m_vulkanObj, displayBufferMeta.semPresentReady, displayBufferMeta.semDisplayReady,
                                    waitSema, waitValue);
    } else {
      LOG_ERR(L"No Display for 0x%08llx:%u", displayBufferMeta.bufferVaddr, displayBufferMeta.bufferSize);
    }
  } else {
    auto image = window.config.displayBuffers[index].lock();
    vulkan::transfer2Display(displayBufferMeta.transferBuffer, m_vulkanObj, swapchain, image->getImage(), image.get(), index);
    vulkan::submitDisplayTransfer(displayBufferMeta.transferBuffer, m_vulkanObj, displayBufferMeta.semPresentReady, displayBufferMeta.semDisplayReady, waitSema,
                                  waitValue);
  }
}

void VideoOut::submitFlip(int handle, int index, int64_t flipArg) {
  OPTICK_EVENT();
  LOG_USE_MODULE(VideoOut);

  m_graphics->submited(); // increase internal counter (wait for flip)

  auto&          window   = m_windows[handle - 1];
  uint32_t const setIndex = window.config.buffers[index];

  LOG_TRACE(L"submitFlip(%d):%u %d", handle, setIndex, index);
  std::unique_lock lock(m_mutexInt);

  transferDisplay(handle, index, nullptr, 0);

  auto& flipStatus = m_windows[index].config.flipStatus;
  ++flipStatus.gcQueueNum;
  flipStatus.flipArg = flipArg;

  auto&      timer   = accessTimer();
  auto const curTime = (uint64_t)(1e9 * timer.getTimeS());

  flipStatus.submitTsc = curTime;
  // window.config.flipStatus.currentBuffer = index; // set after flip, before vblank

  m_messages.push({MessageType::flip, handle - 1, nullptr, setIndex});
  lock.unlock();
  m_condGlfw.notify_one();
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

  for (auto& item: window.eventFlip) {
    (void)EventQueue::KernelTriggerEvent(item, VIDEO_OUT_EVENT_VBLANK, EventQueue::KERNEL_EVFILT_VIDEO_OUT,
                                         reinterpret_cast<void*>(window.config.vblankStatus.count));
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

  *(SceVideoOutBufferAttribute*)attribute = SceVideoOutBufferAttribute {
      .pixelFormat  = SceVideoOutPixelFormat::PIXEL_FORMAT_A8R8G8B8_SRGB, // todo get vulkan pixel_format?
      .tilingMode   = tiling_mode,
      .aspectRatio  = aspect_ratio,
      .width        = width,
      .height       = height,
      .pitchInPixel = pitchInPixel,
  };
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
    LOG_INFO(L"+bufferset[%d] buffer:%d vaddr:0x%08llx", setIndex, n, (uint64_t)addresses[n]);

    auto [format, colorSpace] = vulkan::getDisplayFormat(m_vulkanObj);
    if (!accessGpuMemory().registerDisplayBuffer(bufferSet.buffers[n].bufferVaddr, VkExtent2D {.width = _att->width, .height = _att->height},
                                                 _att->pitchInPixel, format))
      return -1;
  }

  config.flipStatus.currentBuffer =
      createData(m_vulkanObj, m_windows[handle - 1].surface, bufferSet, _att->width, _att->height, accessInitParams()->useVSYNC());
  return setIndex;
}

std::pair<VkQueue, uint32_t> VideoOut::getQueue(vulkan::QueueType type) {

  std::unique_lock const lock(m_mutexInt);

  auto& queueInfo = m_vulkanObj->queues.items[vulkan::getIndex(type)];
  auto  bestIt    = queueInfo.begin();

  // Search for least used
  for (auto it = queueInfo.begin()++; it != queueInfo.end(); ++it) {
    if (it->useCount < bestIt->useCount) {
      bestIt = it;
    }
  }
  // -

  ++bestIt->useCount;
  return std::make_pair(bestIt->queue, bestIt->family);
}

std::thread VideoOut::createGlfwThread() {
  return std::thread([this] {
    util::setThreadName("VideoOut");
    OPTICK_THREAD("VideoOut");

    LOG_USE_MODULE(VideoOut);
    LOG_DEBUG(L"Init glfw");

    glfwInit();

    auto const vblankTime = (uint64_t)(1e6 / 59.0);
    while (!m_stop) {
      std::unique_lock lock(m_mutexInt);
      m_condGlfw.wait_for(lock, std::chrono::microseconds(vblankTime), [=] { return m_stop || !m_messages.empty(); });
      if (m_stop) break;

      if (m_messages.empty()) {
        using namespace std::chrono;

        auto&      timer    = accessTimer();
        auto const curTime  = (uint64_t)(1e6 * timer.getTimeS());
        auto const procTime = timer.queryPerformance();
        for (size_t n = 0; n < m_windows.size(); ++n) {

          vblankEnd(1 + n, curTime, procTime);
        }
        continue;
      }
      auto       item  = m_messages.front();
      auto const index = item.windowIndex;
      switch (item.type) {
        case MessageType::open: {
          auto const title = getTitle(index, 0, 0);

          glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
          glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
          glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
          m_windows[index].window = glfwCreateWindow(m_widthTotal, m_heightTotal, title.c_str(), nullptr, nullptr);

          glfwMakeContextCurrent(m_windows[index].window);
          glfwShowWindow(m_windows[index].window);

          glfwGetWindowSize(m_windows[index].window, (int*)(&m_windows[index].config.resolution.paneWidth),
                            (int*)(&m_windows[index].config.resolution.paneHeight));

          LOG_INFO(L"--> VideoOut Open(%S)| %d:%d", title.c_str(), m_windows[index].config.resolution.paneWidth, m_windows[index].config.resolution.paneHeight);
          if (m_vulkanObj == nullptr) {
            m_vulkanObj = vulkan::initVulkan(m_windows[index].window, m_windows[index].surface, accessInitParams()->enableValidation());
            auto& info  = m_vulkanObj->deviceInfo;

            initManager(info.device, info.physicalDevice, info.instance);
            m_graphics = createGraphics(*this);
          } else {
            vulkan::createSurface(m_vulkanObj, m_windows[index].window, m_windows[index].surface);
          }

          *item.done = true;
          m_condDone.notify_one();
        } break;
        case MessageType::close: {
          glfwDestroyWindow(m_windows[index].window);
          *item.done = true;
          m_condDone.notify_one();
        } break;
        case MessageType::flip: {
          LOG_TRACE(L"-> flip(%d) set:%u buffer:%u", index, item.index, m_windows[index].config.flipStatus.currentBuffer);
          OPTICK_FRAME("VideoOut");
          auto& flipStatus = m_windows[index].config.flipStatus;
          using namespace std::chrono;

          lock.unlock();
          {
            OPTICK_EVENT("Present");
            presentImage(m_vulkanObj, m_windows[index].config.bufferSets[item.index], (uint32_t&)m_windows[index].config.flipStatus.currentBuffer);
          }
          m_graphics->submitDone();
          lock.lock();

          auto&      timer      = accessTimer();
          auto const curTime    = (uint64_t)(1e6 * timer.getTimeS());
          auto const procTime   = timer.queryPerformance();
          auto       elapsed_us = curTime - flipStatus.processTime;

          flipStatus.tsc         = procTime;
          flipStatus.processTime = curTime;
          ++flipStatus.count;
          --flipStatus.gcQueueNum;
          vblankEnd(1 + index, curTime, procTime);

          // Trigger Event Flip
          for (auto& item: m_windows[index].eventFlip) {
            (void)EventQueue::KernelTriggerEvent(item, VIDEO_OUT_EVENT_FLIP, EventQueue::KERNEL_EVFILT_VIDEO_OUT,
                                                 reinterpret_cast<void*>(m_windows[index].config.flipStatus.flipArg));
          }
          // - Flip event

          double const fps   = (m_windows[index].config.fps * 5.0 + (1e6 / (double)elapsed_us)) / 6.0;
          auto         title = getTitle(index + 1, flipStatus.count, round(fps));

          m_windows[index].config.fps = fps;

          glfwSetWindowTitle(m_windows[index].window, title.c_str());
          glfwPollEvents();
          LOG_TRACE(L"<- flip(%d) set:%u buffer:%u", index, item.index, m_windows[index].config.flipStatus.currentBuffer);
        } break;
      }
      m_messages.pop();
    }
    glfwTerminate();
  });
}