#pragma once

#include "vulkanTypes.h"

#include <array>
#include <boost/thread/thread.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

struct SDL_Window;

namespace vulkan {

enum class QueueType : uint8_t {
  graphics = 0,
  compute,
  transfer,
  present,
  numTypes,
};

constexpr std::underlying_type<QueueType>::type getIndex(QueueType type) {
  return (std::underlying_type<QueueType>::type)type;
}

struct QueueInfo {
  VkQueue  queue    = nullptr;
  uint32_t family   = 0;
  size_t   useCount = 0;

  mutable std::mutex mutex; // sync queue submit access

  QueueInfo(VkQueue queue_, uint32_t family_): queue(queue_), family(family_) {}
};

struct Queues {
  std::array<std::vector<std::unique_ptr<QueueInfo>>, getIndex(QueueType::numTypes)> items {}; /// first: VkQueue, second: familyindex
};

struct SwapchainData {

  struct DisplayBuffers {
    uint64_t bufferVaddr = 0;
    uint32_t bufferSize  = 0;
    uint32_t bufferAlign = 0;
  };

  std::vector<DisplayBuffers> buffers;
};

struct SurfaceCapabilities {
  VkSurfaceCapabilitiesKHR        capabilities {};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR>   presentModes;

  bool format_srgb_bgra32  = false;
  bool format_unorm_bgra32 = false;
};

struct VulkanObj {
  std::shared_ptr<vulkan::DeviceInfo> deviceInfo;

  VkDebugUtilsMessengerEXT debugMessenger = nullptr;
  SurfaceCapabilities      surfaceCapabilities;
  Queues                   queues;

  VulkanObj() { deviceInfo = std::make_shared<DeviceInfo>(); }
};

VulkanObj* initVulkan(SDL_Window* window, VkSurfaceKHR& surface, bool useValidation);
void       deinitVulkan(VulkanObj* obj);

void createSurface(VulkanObj* obj, SDL_Window* window, VkSurfaceKHR& surfaceOut);

std::pair<VkFormat, VkColorSpaceKHR> getDisplayFormat(VulkanObj* obj);
} // namespace vulkan
