#pragma once
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

class IImageHandler {
  CLASS_NO_COPY(IImageHandler);
  CLASS_NO_MOVE(IImageHandler);

  protected:
  IImageHandler(VkExtent2D extentWindow, vulkan::QueueInfo* queue): m_extentWindow(extentWindow), m_queue(queue) {}

  VkExtent2D m_extentWindow;
  VkFormat   m_imageFormat;

  vulkan::QueueInfo* m_queue;

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

std::unique_ptr<IImageHandler> createImageHandler(VkDevice vkDevice, VkExtent2D extentWindow, vulkan::QueueInfo* queue);