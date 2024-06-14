#pragma once

#include "vulkanSetup.h"

#include <functional>

class IGraphics;
struct ImageData;

namespace vulkan {
struct PresentData {
  VkImage     swapchainImage = nullptr;
  VkSemaphore displayReady   = nullptr;
  VkSemaphore presentReady   = nullptr;
  uint32_t    index          = 0;
};

std::pair<VkFormat, VkColorSpaceKHR> getDisplayFormat(VulkanObj* obj);

void submitDisplayTransfer(SwapchainData::DisplayBuffers const* displayBuffer, ImageData const& imageData, QueueInfo const* queue, VkSemaphore waitSema,
                           size_t waitValue);

void transfer2Display(SwapchainData::DisplayBuffers const* displayBuffer, ImageData const& imageData, IGraphics* graphics);

void presentImage(ImageData const& imageData, VkSwapchainKHR swapchain, QueueInfo const* queue);

} // namespace vulkan
