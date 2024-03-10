#include "vulkanHelper.h"

#include "core/imports/exports/gpuMemory_types.h"
#include "logging.h"
#include "utility/utility.h"

#include <GLFW/glfw3.h>
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

void createSurface(VulkanObj* obj, GLFWwindow* window, VkSurfaceKHR& surfaceOut) {
  LOG_USE_MODULE(vulkanHelper);
  if (glfwCreateWindowSurface(obj->deviceInfo.instance, window, NULL, &surfaceOut)) {
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

uint32_t createData(VulkanObj* obj, VkSurfaceKHR surface, vulkan::SwapchainData& swapchainData, uint32_t width, uint32_t height, bool enableVsync) {
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

    for (uint8_t i = 0; i < numImages; ++i) {
      swapchainData.buffers[i].image = images[i];
      vkCreateSemaphore(obj->deviceInfo.device, &semCreateInfo, nullptr, &swapchainData.buffers[i].semDisplayReady);
      vkCreateSemaphore(obj->deviceInfo.device, &semCreateInfo, nullptr, &swapchainData.buffers[i].semPresentReady);
    }
  }

  // Flip data
  {
    VkCommandPoolCreateInfo const poolInfo {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = obj->queues.items[getIndex(QueueType::graphics)][0].family,
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

  uint32_t curBufferIndex = 0;
  // Get first display buffer
  while (true) {
    auto result = vkAcquireNextImageKHR(obj->deviceInfo.device, swapchainData.swapchain, UINT64_MAX, swapchainData.buffers[curBufferIndex].semDisplayReady,
                                        VK_NULL_HANDLE, &curBufferIndex);
    if (result != VK_SUCCESS) {
      if (result == VK_NOT_READY) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms); // Not init yet or whatever
        continue;
      } else
        LOG_CRIT(L"vkAcquireNextImageKHR err:%S", string_VkResult(result));
    }
    break;
  }
  // -
  BOOST_ASSERT_MSG(curBufferIndex == 0, "AcquireNextImage init returned unexpected value");

  return curBufferIndex;
}

void submitDisplayTransfer(VkCommandBuffer cmdBuffer, VulkanObj* obj, VkSemaphore semPresentReady, VkSemaphore displayReady, VkSemaphore waitSema,
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
  VkSemaphore          sems[]      = {displayReady, waitSema};

  VkSubmitInfo const submitInfo {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = waitSema != nullptr ? &timelineInfo : nullptr,

      .waitSemaphoreCount = waitCount,
      .pWaitSemaphores    = sems,
      .pWaitDstStageMask  = waitStage,

      .commandBufferCount = 1,
      .pCommandBuffers    = &cmdBuffer,

      .signalSemaphoreCount = 1,
      .pSignalSemaphores    = &semPresentReady,
  };

  if (VkResult result = vkQueueSubmit(obj->queues.items[getIndex(QueueType::graphics)][0].queue, 1, &submitInfo, VK_NULL_HANDLE); result != VK_SUCCESS) {
    LOG_CRIT(L"Couldn't vkQueueSubmit Transfer %S", string_VkResult(result));
  }
}

void transfer2Display(VkCommandBuffer cmdBuffer, VulkanObj* obj, vulkan::SwapchainData& swapchain, VkImage displayImage, IGpuImageObject* image,
                      uint32_t index) {
  LOG_USE_MODULE(vulkanHelper);

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

  {
    VkImageMemoryBarrier const barrier {.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                        .pNext               = nullptr,
                                        .srcAccessMask       = 0,
                                        .dstAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
                                        .oldLayout           = image->getImageLayout(),
                                        .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

                                        .image            = image->getImage(),
                                        .subresourceRange = image->getSubresource()};

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    image->setImageLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  }

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

        .image            = swapchain.buffers[index].image,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }

  VkImageBlit const blit {
      .srcSubresource =
          {
              .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel       = 0,
              .baseArrayLayer = 0,
              .layerCount     = 1,
          },
      .srcOffsets = {{0, 0, 0}, {(int32_t)swapchain.extent2d.width, (int32_t)swapchain.extent2d.height, 1}},
      .dstSubresource =
          {
              .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel       = 0,
              .baseArrayLayer = 0,
              .layerCount     = 1,
          },
      .dstOffsets = {{0, 0, 0}, {(int32_t)swapchain.extent2d.width, (int32_t)swapchain.extent2d.height, 1}},
  };

  vkCmdBlitImage(cmdBuffer, displayImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchain.buffers[index].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                 VK_FILTER_LINEAR);

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

        .image            = swapchain.buffers[index].image,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }
  // - Present layout

  // End CmdBuffer -> Submit
  if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
    LOG_CRIT(L"Couldn't end commandbuffer");
  }
  // -
}

void presentImage(VulkanObj* obj, vulkan::SwapchainData& swapchain, uint32_t& index) {
  LOG_USE_MODULE(vulkanHelper);

  VkPresentInfoKHR const presentInfo {
      .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext              = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores    = &swapchain.buffers[index].semPresentReady,
      .swapchainCount     = 1,
      .pSwapchains        = &swapchain.swapchain,
      .pImageIndices      = &index,
      .pResults           = nullptr,
  };

  {
    OPTICK_GPU_FLIP(&swapchain.swapchain);
    OPTICK_CATEGORY("Present", Optick::Category::Wait);
    vkQueuePresentKHR(obj->queues.items[getIndex(QueueType::present)][0].queue, &presentInfo);
  }

  uint32_t swapchainIndex = ++index;
  if (swapchainIndex >= swapchain.buffers.size()) swapchainIndex = 0;
  auto result =
      vkAcquireNextImageKHR(obj->deviceInfo.device, swapchain.swapchain, UINT64_MAX, swapchain.buffers[swapchainIndex].semDisplayReady, VK_NULL_HANDLE, &index);

  BOOST_ASSERT_MSG(swapchainIndex == index, "AcquireNextImage returned unexpected value");
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