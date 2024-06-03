#include "vulkanHelper.h"

#include "../imageHandler.h"
#include "core/initParams/initParams.h"
#include "logging.h"
#include "utility/utility.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <algorithm>
#include <array>
#include <assert.h>
#include <boost/assert.hpp>
#include <chrono>
#include <format>
#include <graphics.h>
#include <mutex>
#include <optick.h>
#include <string>
#include <thread>
#include <vulkan/vk_enum_string_helper.h>

LOG_DEFINE_MODULE(vulkanHelper);

namespace vulkan {

void createSurface(VulkanObj* obj, SDL_Window* window, VkSurfaceKHR& surfaceOut) {
  LOG_USE_MODULE(vulkanHelper);
  if (SDL_Vulkan_CreateSurface(window, obj->deviceInfo->instance, &surfaceOut)) {
    LOG_CRIT(L"Couldn't create surface");
  }
}

std::pair<VkFormat, VkColorSpaceKHR> getDisplayFormat(VulkanObj* obj) {
  if (obj->surfaceCapabilities.formats.empty()) {
    return {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }

  std::vector<std::pair<uint16_t, VkFormat>> formatsSrgb;
  std::vector<std::pair<uint16_t, VkFormat>> formatsUnorm;

  formatsSrgb.reserve(obj->surfaceCapabilities.formats.size());
  formatsUnorm.reserve(obj->surfaceCapabilities.formats.size());

  for (auto const& format: obj->surfaceCapabilities.formats) {
    switch (format.format) {
      case VK_FORMAT_B8G8R8A8_SRGB: formatsSrgb.push_back({1, VK_FORMAT_B8G8R8A8_SRGB}); break;
      case VK_FORMAT_R8G8B8A8_SRGB: formatsSrgb.push_back({2, VK_FORMAT_R8G8B8A8_SRGB}); break;
      case VK_FORMAT_B8G8R8A8_UNORM: formatsUnorm.push_back({1, VK_FORMAT_B8G8R8A8_UNORM}); break;
      case VK_FORMAT_B8G8R8A8_SNORM: formatsUnorm.push_back({2, VK_FORMAT_B8G8R8A8_SNORM}); break;
      default: break;
    }
  }

  sort(formatsSrgb.begin(), formatsSrgb.end());
  sort(formatsUnorm.begin(), formatsUnorm.end());

  if (accessInitParams()->enableBrightness()) {
    if (formatsSrgb.empty()) {
      LOG_USE_MODULE(vulkanHelper);
      for (auto const& format: obj->surfaceCapabilities.formats) {
        LOG_ERR(L"format %S", string_VkFormat(format.format));
      }
      LOG_CRIT(L"VK_FORMAT_B8G8R8A8_SRGB not supported");
    }
    return {formatsSrgb[0].second, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }

  if (formatsUnorm.empty()) {
    LOG_USE_MODULE(vulkanHelper);
    for (auto const& format: obj->surfaceCapabilities.formats) {
      LOG_ERR(L"format %S", string_VkFormat(format.format));
    }
    LOG_CRIT(L"VK_FORMAT_B8G8R8A8_UNORM not supported");
  }
  return {formatsUnorm[0].second, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
}

void submitDisplayTransfer(SwapchainData::DisplayBuffers const* displayBuffer, ImageData const& imageData, QueueInfo const* queue, VkSemaphore waitSema,
                           size_t waitValue) {
  LOG_USE_MODULE(vulkanHelper);

  if (vkEndCommandBuffer(imageData.cmdBuffer) != VK_SUCCESS) {
    LOG_CRIT(L"Couldn't end commandbuffer");
  }

  size_t   waitValues[] = {0, waitValue};
  uint32_t waitCount    = waitSema != nullptr ? 2 : 1;

  VkTimelineSemaphoreSubmitInfo const timelineInfo {
      .sType                   = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
      .waitSemaphoreValueCount = waitCount,
      .pWaitSemaphoreValues    = waitValues,
  };

  VkPipelineStageFlags waitStage[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT};
  VkSemaphore          sems[]      = {imageData.semImageReady, waitSema};

  VkSubmitInfo const submitInfo {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = waitSema != nullptr ? &timelineInfo : nullptr,

      .waitSemaphoreCount = waitCount,
      .pWaitSemaphores    = sems,
      .pWaitDstStageMask  = waitStage,

      .commandBufferCount = 1,
      .pCommandBuffers    = &imageData.cmdBuffer,

      .signalSemaphoreCount = 1,
      .pSignalSemaphores    = &imageData.semImageCopied,
  };

  {
    std::unique_lock lock(queue->mutex);
    if (VkResult result = vkQueueSubmit(queue->queue, 1, &submitInfo, imageData.submitFence); result != VK_SUCCESS) {
      LOG_CRIT(L"Couldn't vkQueueSubmit Transfer %S", string_VkResult(result));
    }
  }
}

void transfer2Display(SwapchainData::DisplayBuffers const* displayBuffer, ImageData const& imageData, IGraphics* graphics) {
  LOG_USE_MODULE(vulkanHelper);

  {
    VkImageMemoryBarrier const barrier {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = 0,
        .dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image            = imageData.swapchainImage,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    vkCmdPipelineBarrier(imageData.cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }

  graphics->copyDisplayBuffer(displayBuffer->bufferVaddr, imageData.cmdBuffer, imageData.swapchainImage, imageData.extent); // let gpumemorymanager decide

  {
    // Change to Present Layout
    VkImageMemoryBarrier const barrier {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask       = 0,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image            = imageData.swapchainImage,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    vkCmdPipelineBarrier(imageData.cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }
  // - Present layout
}

void presentImage(ImageData const& imageData, VkSwapchainKHR swapchain, QueueInfo const* queue) {
  LOG_USE_MODULE(vulkanHelper);

  VkPresentInfoKHR const presentInfo {
      .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext              = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores    = &imageData.semImageCopied,
      .swapchainCount     = 1,
      .pSwapchains        = &swapchain,
      .pImageIndices      = &imageData.index,
      .pResults           = nullptr,
  };

  {
    OPTICK_GPU_FLIP(&swapchain);
    OPTICK_CATEGORY("Present", Optick::Category::Wait);

    std::unique_lock lock(queue->mutex);
    vkQueuePresentKHR(queue->queue, &presentInfo);
  }
}
} // namespace vulkan