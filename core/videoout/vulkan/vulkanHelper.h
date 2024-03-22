#pragma once

#include "vulkanSetup.h"

#include <functional>

class IGraphics;

namespace vulkan {
struct PresentData {
  VkImage     swapchainImage = nullptr;
  VkSemaphore displayReady   = nullptr;
  VkSemaphore presentReady   = nullptr;
  uint32_t    index          = 0;
};

void submitDisplayTransfer(VulkanObj* obj, SwapchainData::DisplayBuffers const* displayBuffer, PresentData* presentData, VkSemaphore waitSema,
                           size_t waitValue);

PresentData transfer2Display(SwapchainData::DisplayBuffers const* displayBuffer, VulkanObj* obj, vulkan::SwapchainData& swapchain, IGraphics* graphics);

bool presentImage(VulkanObj* obj, SwapchainData& swapchain, vulkan::PresentData const& presentData);

void waitFlipped(VulkanObj* obj); /// Call before submit
} // namespace vulkan
