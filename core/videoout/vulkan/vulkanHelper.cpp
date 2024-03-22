#include "vulkanHelper.h"

#include "core/imports/exports/graphics.h"
#include "logging.h"
#include "utility/utility.h"

#include <SDL.h>
#include <SDL_vulkan.h>
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

  VkFormat        format          = obj->surfaceCapabilities.formats[0].format;
  VkColorSpaceKHR imageColorSpace = obj->surfaceCapabilities.formats[0].colorSpace;
  if (obj->surfaceCapabilities.format_unorm_bgra32) {
    format          = VK_FORMAT_B8G8R8A8_UNORM;
    imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  } else if (obj->surfaceCapabilities.format_srgb_bgra32) {
    format          = VK_FORMAT_B8G8R8A8_SRGB;
    imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  }
  return {format, imageColorSpace};
}

void createData(VulkanObj* obj, VkSurfaceKHR surface, vulkan::SwapchainData& swapchainData, uint32_t width, uint32_t height, bool enableVsync) {
  LOG_USE_MODULE(vulkanHelper);

  uint32_t const numBuffers = swapchainData.buffers.size();

  if (numBuffers > obj->surfaceCapabilities.capabilities.maxImageCount || numBuffers < obj->surfaceCapabilities.capabilities.minImageCount) {
    LOG_CRIT(L"numBuffers:%d outside %d:%d", numBuffers, obj->surfaceCapabilities.capabilities.minImageCount,
             obj->surfaceCapabilities.capabilities.maxImageCount);
  }

  swapchainData.extent2d.width =
      std::clamp(width, obj->surfaceCapabilities.capabilities.minImageExtent.width, obj->surfaceCapabilities.capabilities.maxImageExtent.width);
  swapchainData.extent2d.height =
      std::clamp(height, obj->surfaceCapabilities.capabilities.minImageExtent.height, obj->surfaceCapabilities.capabilities.maxImageExtent.height);

  auto [displayFormat, displayColorSpace] = getDisplayFormat(obj);
  swapchainData.format                    = displayFormat;

  VkSwapchainCreateInfoKHR const createInfo {
      .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext                 = nullptr,
      .flags                 = 0,
      .surface               = surface,
      .minImageCount         = numBuffers,
      .imageFormat           = swapchainData.format,
      .imageColorSpace       = displayColorSpace,
      .imageExtent           = swapchainData.extent2d,
      .imageArrayLayers      = 1,
      .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices   = nullptr,
      .preTransform          = obj->surfaceCapabilities.capabilities.currentTransform,
      .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode           = enableVsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR,
      .clipped               = VK_TRUE,
      .oldSwapchain          = nullptr,
  };

  vkCreateSwapchainKHR(obj->deviceInfo.device, &createInfo, nullptr, &swapchainData.swapchain);

  uint32_t numImages = numBuffers;
  vkGetSwapchainImagesKHR(obj->deviceInfo.device, swapchainData.swapchain, &numImages, nullptr);
  BOOST_ASSERT_MSG(numImages == numBuffers, "Swapchain created more images");

  { // Create Semaphore + imageView
    std::vector<VkImage> images(numImages);
    vkGetSwapchainImagesKHR(obj->deviceInfo.device, swapchainData.swapchain, &numImages, images.data());

    VkSemaphoreCreateInfo const semCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
    };

    VkFenceCreateInfo const fenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = 0,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (uint8_t i = 0; i < numImages; ++i) {
      auto& buffer = swapchainData.buffers[i];
      buffer.image = images[i];

      vkCreateSemaphore(obj->deviceInfo.device, &semCreateInfo, nullptr, &buffer.semDisplayReady);
      vkCreateSemaphore(obj->deviceInfo.device, &semCreateInfo, nullptr, &buffer.semPresentReady);

      vkCreateFence(obj->deviceInfo.device, &fenceCreateInfo, nullptr, &buffer.bufferFence);
    }
  }

  // Flip data
  {
    VkCommandPoolCreateInfo const poolInfo {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = obj->queues.items[getIndex(QueueType::graphics)][0]->family,
    };

    if (auto result = vkCreateCommandPool(obj->deviceInfo.device, &poolInfo, nullptr, &swapchainData.commandPool); result != VK_SUCCESS) {
      LOG_CRIT(L"Couldn't create commandpool(graphics): %d", result);
    }
  }
  {
    VkCommandBufferAllocateInfo const allocInfo {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = swapchainData.commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    for (uint8_t i = 0; i < numImages; ++i) {
      if (auto result = vkAllocateCommandBuffers(obj->deviceInfo.device, &allocInfo, &swapchainData.buffers[i].transferBuffer); result != VK_SUCCESS) {
        LOG_CRIT(L"Couldn't create commandbuffers(graphics): %d", result);
      }
    }
  }
  // - Flip Data
}

void submitDisplayTransfer(VulkanObj* obj, SwapchainData::DisplayBuffers const* displayBuffer, PresentData* presentData, VkSemaphore waitSema,
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
  VkSemaphore          sems[]      = {presentData->displayReady, waitSema};

  presentData->presentReady = displayBuffer->semPresentReady;

  VkSubmitInfo const submitInfo {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = waitSema != nullptr ? &timelineInfo : nullptr,

      .waitSemaphoreCount = waitCount,
      .pWaitSemaphores    = sems,
      .pWaitDstStageMask  = waitStage,

      .commandBufferCount = 1,
      .pCommandBuffers    = &displayBuffer->transferBuffer,

      .signalSemaphoreCount = 1,
      .pSignalSemaphores    = &presentData->presentReady,
  };

  {
    auto& queue = obj->queues.items[getIndex(QueueType::present)][0];

    std::unique_lock lock(queue->mutex);
    if (VkResult result = vkQueueSubmit(queue->queue, 1, &submitInfo, displayBuffer->bufferFence); result != VK_SUCCESS) {
      LOG_CRIT(L"Couldn't vkQueueSubmit Transfer %S", string_VkResult(result));
    }
  }
}

PresentData transfer2Display(SwapchainData::DisplayBuffers const* displayBuffer, VulkanObj* obj, vulkan::SwapchainData& swapchain, IGraphics* graphics) {
  LOG_USE_MODULE(vulkanHelper);

  // Wait on prev present (only one image is allowed )
  {
    boost::unique_lock lock(swapchain.mutexPresent);
    swapchain.condPresent.wait(lock, [&swapchain] { return swapchain.waitId == swapchain.presentId; });
    ++swapchain.waitId;
  }
  // -

  PresentData presentData;

  vkWaitForFences(obj->deviceInfo.device, 1, &displayBuffer->bufferFence, VK_TRUE, UINT64_MAX);
  vkResetFences(obj->deviceInfo.device, 1, &displayBuffer->bufferFence);

  // Get swapchain image
  presentData.displayReady = displayBuffer->semDisplayReady;
  {
    int      n      = 2;
    VkResult result = VK_SUCCESS;
    for (; n >= 0; --n) {
      if (result = vkAcquireNextImageKHR(obj->deviceInfo.device, swapchain.swapchain, UINT64_MAX, presentData.displayReady, VK_NULL_HANDLE, &presentData.index);
          result != VK_SUCCESS) {
        if (result == VK_NOT_READY) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
      } else
        break;
    }
    if (n <= 0) {
      LOG_ERR(L"vkAcquireNextImageKHR %S", string_VkResult(result));
      return {};
    }
  }
  presentData.swapchainImage = swapchain.buffers[presentData.index].image;
  // -

  auto cmdBuffer = displayBuffer->transferBuffer;

  // Wait and begin command buffer
  vkResetCommandBuffer(cmdBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

  // Transfer
  VkCommandBufferBeginInfo const beginInfo {
      .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
      .pInheritanceInfo = nullptr,
  };

  if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
    LOG_CRIT(L"Error vkBeginCommandBuffer");
  }
  // - begin command buffer

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

        .image            = presentData.swapchainImage,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }

  graphics->copyDisplayBuffer(displayBuffer->bufferVaddr, cmdBuffer, presentData.swapchainImage, swapchain.extent2d); // let gpumemorymanager decide

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

        .image            = presentData.swapchainImage,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }
  // - Present layout

  // End CmdBuffer -> Submit
  if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
    LOG_CRIT(L"Couldn't end commandbuffer");
  }
  // -

  return presentData;
}

bool presentImage(VulkanObj* obj, vulkan::SwapchainData& swapchain, vulkan::PresentData const& presentData) {
  LOG_USE_MODULE(vulkanHelper);

  VkPresentInfoKHR const presentInfo {
      .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext              = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores    = &presentData.presentReady,
      .swapchainCount     = 1,
      .pSwapchains        = &swapchain.swapchain,
      .pImageIndices      = &presentData.index,
      .pResults           = nullptr,
  };

  {
    OPTICK_GPU_FLIP(&swapchain.swapchain);
    OPTICK_CATEGORY("Present", Optick::Category::Wait);

    auto& queue = obj->queues.items[getIndex(QueueType::present)][0];

    std::unique_lock lock(queue->mutex);
    vkQueuePresentKHR(queue->queue, &presentInfo);
  }

  ++swapchain.presentId;
  swapchain.condPresent.notify_all();

  return true;
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
