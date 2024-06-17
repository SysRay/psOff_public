#pragma once
#include <vulkan/vulkan_core.h>

namespace vulkan {

struct DeviceInfo {
  VkInstance       instance       = nullptr;
  VkPhysicalDevice physicalDevice = nullptr;
  VkDevice         device         = nullptr;

  VkPhysicalDeviceProperties devProperties;
};
} // namespace vulkan