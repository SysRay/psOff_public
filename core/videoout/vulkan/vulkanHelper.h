#pragma once

#include "vulkanSetup.h"

#include <functional>

class IGraphics;

namespace vulkan {
void submitDisplayTransfer(VulkanObj* obj, SwapchainData::DisplayBuffers* displayBuffer, VkSemaphore waitSema, size_t waitValue);

void transfer2Display(VkCommandBuffer cmdBuffer, VulkanObj* obj, vulkan::SwapchainData& swapchain, uint32_t index, uint64_t vaddrDisplayBuffer,
                      IGraphics* graphics);

bool presentImage(VulkanObj* obj, SwapchainData& swapchain, uint32_t& index);

void waitFlipped(VulkanObj* obj); /// Call before submit
} // namespace vulkan
