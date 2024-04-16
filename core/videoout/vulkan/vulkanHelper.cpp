#include "vulkanHelper.h"

#include "../imageHandler.h"
#include "core/imports/exports/graphics.h"
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
#include <mutex>
#include <optick.h>
#include <string>
#include <thread>
#include <vulkan/vk_enum_string_helper.h>

LOG_DEFINE_MODULE(vulkanHelper);

namespace vulkan {

void createSurface(VulkanObj* obj, SDL_Window* window, VkSurfaceKHR& surfaceOut) {
  LOG_USE_MODULE(vulkanHelper);
  if (SDL_Vulkan_CreateSurface(window, obj->deviceInfo.instance, &surfaceOut)) {
    LOG_CRIT(L"Couldn't create surface");
  }
}

std::pair<VkFormat, VkColorSpaceKHR> getDisplayFormat(VulkanObj* obj) {
  if (obj->surfaceCapabilities.formats.empty()) {
    return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }
  bool found = false;
  for (auto const& format: obj->surfaceCapabilities.formats) {
    if (format.format == VK_FORMAT_R8G8B8A8_SRGB) {
      found = true;
    }
  }

  if (!found) {
    LOG_USE_MODULE(vulkanHelper);
    for (auto const& format: obj->surfaceCapabilities.formats) {
      LOG_ERR(L"format %S", string_VkFormat(format.format));
    }
    LOG_CRIT(L"VK_FORMAT_B8G8R8A8_SRGB not supported");
  }
  return {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
}

void submitDisplayTransfer(SwapchainData::DisplayBuffers const* displayBuffer, ImageData const& imageData, QueueInfo const* queue, VkSemaphore waitSema,
                           size_t waitValue) {
  LOG_USE_MODULE(vulkanHelper);

  size_t   waitValues[] = {0, waitValue};
  uint32_t waitCount    = waitSema != nullptr ? 2 : 1;

  VkTimelineSemaphoreSubmitInfo const timelineInfo {
      .sType                   = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
      .waitSemaphoreValueCount = waitCount,
      .pWaitSemaphoreValues    = waitValues,
  };

  VkPipelineStageFlags waitStage[] = {VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT};
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

  // End CmdBuffer -> Submit
  if (vkEndCommandBuffer(imageData.cmdBuffer) != VK_SUCCESS) {
    LOG_CRIT(L"Couldn't end commandbuffer");
  }
  // -
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

// ### Vulkan EXT Function Definitions
VKAPI_ATTR VkResult VKAPI_CALL vkCreateShadersEXT(VkDevice device, uint32_t createInfoCount, const VkShaderCreateInfoEXT* pCreateInfos,
                                                  const VkAllocationCallbacks* pAllocator, VkShaderEXT* pShaders) {
  static auto fn = (PFN_vkCreateShadersEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCreateShadersEXT");
  return fn(device, createInfoCount, pCreateInfos, pAllocator, pShaders);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthClipNegativeOneToOneEXT(VkCommandBuffer commandBuffer, VkBool32 negativeOneToOne) {
  static auto fn = (PFN_vkCmdSetDepthClipNegativeOneToOneEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetDepthClipNegativeOneToOneEXT");
  fn(commandBuffer, negativeOneToOne);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetColorWriteEnableEXT(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkBool32* pColorWriteEnables) {
  static auto fn = (PFN_vkCmdSetColorWriteEnableEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetColorWriteEnableEXT");
  fn(commandBuffer, attachmentCount, pColorWriteEnables);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetColorWriteMaskEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount,
                                                     const VkColorComponentFlags* pColorWriteMasks) {
  static auto fn = (PFN_vkCmdSetColorWriteMaskEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetColorWriteMaskEXT");
  fn(commandBuffer, firstAttachment, attachmentCount, pColorWriteMasks);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetColorBlendEnableEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount,
                                                       const VkBool32* pColorBlendEnables) {
  static auto fn = (PFN_vkCmdSetColorBlendEnableEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetColorBlendEnableEXT");
  fn(commandBuffer, firstAttachment, attachmentCount, pColorBlendEnables);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetColorBlendEquationEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount,
                                                         const VkColorBlendEquationEXT* pColorBlendEquations) {
  static auto fn = (PFN_vkCmdSetColorBlendEquationEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetColorBlendEquationEXT");
  fn(commandBuffer, firstAttachment, attachmentCount, pColorBlendEquations);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetPolygonModeEXT(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode) {
  static auto fn = (PFN_vkCmdSetPolygonModeEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetPolygonModeEXT");
  fn(commandBuffer, polygonMode);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetRasterizationSamplesEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits rasterizationSamples) {
  static auto fn = (PFN_vkCmdSetRasterizationSamplesEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetRasterizationSamplesEXT");
  fn(commandBuffer, rasterizationSamples);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetAlphaToCoverageEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToCoverageEnable) {
  static auto fn = (PFN_vkCmdSetAlphaToCoverageEnableEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetAlphaToCoverageEnableEXT");
  fn(commandBuffer, alphaToCoverageEnable);
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetAlphaToOneEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToOneEnable) {
  static auto fn = (PFN_vkCmdSetAlphaToOneEnableEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkCmdSetAlphaToOneEnableEXT");
  fn(commandBuffer, alphaToOneEnable);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryHostPointerPropertiesEXT(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, const void* pHostPointer,
                                                                   VkMemoryHostPointerPropertiesEXT* pMemoryHostPointerProperties) {
  static auto fn = (PFN_vkGetMemoryHostPointerPropertiesEXT)vkGetInstanceProcAddr(vulkan::getVkInstance(), "vkGetMemoryHostPointerPropertiesEXT");
  return fn(device, handleType, pHostPointer, pMemoryHostPointerProperties);
}
