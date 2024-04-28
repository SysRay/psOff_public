#pragma once
#include "core/videoout/vulkan/vulkanTypes.h"
#include "utility/utility.h"

#include <optional>
#include <vulkan/vulkan_core.h>

struct ImageData {
  VkImage         swapchainImage = nullptr;
  VkSemaphore     semImageReady  = nullptr; /// image is ready for operation
  VkSemaphore     semImageCopied = nullptr; /// Image has been copied, ready to swap
  VkCommandBuffer cmdBuffer      = nullptr; /// Commandbuffer to  be used for transer etc. Already called begin!

  VkFence submitFence; /// used for the cmdBuffer queue submit

  VkExtent2D extent;
  uint32_t   index = 0;
};

namespace vulkan {
struct VulkanObj;
struct QueueInfo;
} // namespace vulkan

class ImageHandlerCB {
  CLASS_NO_COPY(ImageHandlerCB);
  CLASS_NO_MOVE(ImageHandlerCB);

  protected:
  ImageHandlerCB() = default;

  public:
  virtual ~ImageHandlerCB() = default;

  virtual VkExtent2D getWindowSize() = 0;
};

class IImageHandler {
  CLASS_NO_COPY(IImageHandler);
  CLASS_NO_MOVE(IImageHandler);

  protected:
  IImageHandler(VkExtent2D extentWindow, vulkan::QueueInfo* queue, ImageHandlerCB* callback)
      : m_extentWindow(extentWindow), m_queue(queue), m_callback(callback) {}

  VkExtent2D m_extentWindow;
  VkFormat   m_imageFormat;

  vulkan::QueueInfo* m_queue;

  ImageHandlerCB* m_callback;

  public:
  virtual ~IImageHandler() = default;

  auto getExtent() const { return m_extentWindow; }

  auto getFormat() const { return m_imageFormat; }

  auto getQueue() const { return m_queue; }

  virtual void init(vulkan::VulkanObj* obj, VkSurfaceKHR surface) = 0;

  virtual void deinit() = 0;
  virtual void stop()   = 0;

  virtual std::optional<ImageData> getImage_blocking()           = 0;
  virtual void                     notify_done(ImageData const&) = 0;

  virtual VkSwapchainKHR getSwapchain() const = 0;
};

std::unique_ptr<IImageHandler> createImageHandler(std::shared_ptr<vulkan::DeviceInfo>& deviceInfo, VkExtent2D extentWindow, vulkan::QueueInfo* queue,
                                                  ImageHandlerCB* callback);