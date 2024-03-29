#include "vulkanSetup.h"

#include "logging.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <format>
#include <utility/utility.h>
#include <vulkan/vk_enum_string_helper.h>

LOG_DEFINE_MODULE(vulkanSetup);

constexpr std::array<VkValidationFeatureDisableEXT, 0> disabledValidationFeatures {};

constexpr std::array enabledValidationFeatures {VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT};

constexpr std::array requiredExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME, VK_KHR_MAINTENANCE_2_EXTENSION_NAME, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME, VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,
    VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME, VK_EXT_SEPARATE_STENCIL_USAGE_EXTENSION_NAME, VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
    // VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME, "VK_KHR_external_memory_win32"};

namespace vulkan {
struct VulkanExtensions {
  bool enableValidationLayers = true;

  std::vector<const char*> requiredExtensions;

  std::vector<VkExtensionProperties> availableExtensions;
  std::vector<const char*>           requiredLayers;
  std::vector<VkLayerProperties>     availableLayers;
};

struct VulkanQueues {
  struct QueueInfo {
    uint32_t family = 0;
    uint32_t index  = 0;
    uint32_t count  = 0;

    bool graphics = false;
    bool compute  = false;
    bool transfer = false;
    bool present  = false;
  };

  uint32_t               familyCount = 0;
  std::vector<uint32_t>  familyUsed;
  std::vector<QueueInfo> graphics;
  std::vector<QueueInfo> compute;
  std::vector<QueueInfo> transfer;
  std::vector<QueueInfo> present;
};

VKAPI_ATTR VkResult VKAPI_CALL createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
                                                            const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* messenger) {

  if (auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")); func != nullptr) {
    return func(instance, createInfo, allocator, messenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* /*user_data*/) {
  LOG_USE_MODULE(vulkanSetup);

  const char* severityStr = nullptr;
  bool        skip        = false;
  bool        error       = false;
  bool        debugPrintf = false;

  switch (message_severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      severityStr = "V";
      skip        = true;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      if ((messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0 && strcmp(callback_data->pMessageIdName, "UNASSIGNED-DEBUG-PRINTF") == 0) {
        debugPrintf = true;
        skip        = true;
      } else {
        severityStr = "I";
      }
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: severityStr = "W"; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      severityStr = "E";
      error       = true;
      break;
    default: severityStr = "?";
  }

  if (error) {
    LOG_WARN(L"[Vulkan][%S][%u]: %S", severityStr, static_cast<uint32_t>(messageTypes), callback_data->pMessage);
  } else if (!skip) {
    LOG_DEBUG(L"[Vulkan][%S][%u]: %S", severityStr, static_cast<uint32_t>(messageTypes), callback_data->pMessage);
  }

  if (debugPrintf) {
    auto strs = util::splitString(std::string(callback_data->pMessage), U'|');
    if (!strs.empty()) {
      LOG_DEBUG(L"%S", std::string(strs.back()).data());
    }
  }

  return VK_FALSE;
}

VulkanExtensions getExtensions(SDL_Window* window, bool enableValidation) {
  LOG_USE_MODULE(vulkanSetup);

  uint32_t countAvailableExtensions = 0;
  uint32_t countAvailableLayers     = 0;
  uint32_t countRequiredExtensions  = 0;

  VulkanExtensions r = {.enableValidationLayers = enableValidation};

  SDL_Vulkan_GetInstanceExtensions(window, &countRequiredExtensions, NULL);
  auto extensions = static_cast<const char**>(SDL_malloc(sizeof(char*) * countRequiredExtensions));
  SDL_Vulkan_GetInstanceExtensions(window, &countRequiredExtensions, extensions);
  for (size_t n = 0; n < countRequiredExtensions; n++) {
    r.requiredExtensions.push_back(extensions[n]);
  }
  SDL_free(extensions);

  vkEnumerateInstanceExtensionProperties(nullptr, &countAvailableExtensions, nullptr);
  r.availableExtensions = std::vector<VkExtensionProperties>(countAvailableExtensions, VkExtensionProperties {});
  vkEnumerateInstanceExtensionProperties(nullptr, &countAvailableExtensions, r.availableExtensions.data());

  if (std::find_if(r.availableExtensions.begin(), r.availableExtensions.end(),
                   [](VkExtensionProperties s) { return strcmp(s.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0; }) != r.availableExtensions.end()) {
    r.requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  } else {
    r.enableValidationLayers = false;
  }

  for (size_t n = 0; n < r.requiredExtensions.size(); ++n) {
    LOG_INFO(L"SDL2 required extension: %S", r.requiredExtensions[n]);
  }

  for (const auto& ext: r.availableExtensions) {
    LOG_DEBUG(L"Vulkan available extension: %S specVer:%u", ext.extensionName, ext.specVersion);
  }

  vkEnumerateInstanceLayerProperties(&countAvailableLayers, nullptr);
  r.availableLayers = std::vector<VkLayerProperties>(countAvailableLayers, VkLayerProperties {});
  vkEnumerateInstanceLayerProperties(&countAvailableLayers, r.availableLayers.data());

  for (const auto& l: r.availableLayers) {
    LOG_DEBUG(L"Vulkan available layer:%S specVer:%u.%u implVer:%u (%S)", l.layerName, VK_API_VERSION_MAJOR(l.specVersion), VK_API_VERSION_MINOR(l.specVersion),
              l.implementationVersion, l.description);
  }

  if (r.enableValidationLayers) {
    r.requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
    for (auto const l: r.requiredLayers) {
      if (std::find_if(r.availableLayers.begin(), r.availableLayers.end(), [&l](auto s) { return strcmp(s.layerName, l) == 0; }) == r.availableLayers.end()) {
        LOG_INFO(L"no validation layer:%S", l);
        r.enableValidationLayers = false;
        break;
      }
    }
  }

  if (r.enableValidationLayers) {
    vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &countAvailableExtensions, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(countAvailableExtensions, VkExtensionProperties {});
    vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &countAvailableExtensions, availableExtensions.data());

    for (const auto& ext: availableExtensions) {
      LOG_DEBUG(L"VK_LAYER_KHRONOS_validation available extension: %S version:%u", ext.extensionName, ext.specVersion);
    }

    if (std::find_if(availableExtensions.begin(), availableExtensions.end(),
                     [](auto s) { return strcmp(s.extensionName, "VK_EXT_validation_features") == 0; }) != availableExtensions.end()) {
      r.requiredExtensions.push_back("VK_EXT_validation_features");
    } else {
      r.enableValidationLayers = false;
    }
  }

  return r;
}

VulkanQueues findQueues(VkPhysicalDevice device, VkSurfaceKHR surface) {
  LOG_USE_MODULE(vulkanSetup);
  VulkanQueues qs;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &qs.familyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(qs.familyCount, VkQueueFamilyProperties {});
  vkGetPhysicalDeviceQueueFamilyProperties(device, &qs.familyCount, queueFamilies.data());

  std::vector<VulkanQueues::QueueInfo> queueInfos;
  qs.familyUsed.resize(queueFamilies.size());

  for (uint32_t n = 0; n < queueFamilies.size(); ++n) {
    VkBool32 presentationSupported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, n, surface, &presentationSupported);

    auto const& queue = queueFamilies[n];
    LOG_DEBUG(L"queue family[%u]: %S [count:%u] [present=%S]", n, string_VkQueueFlags(queue.queueFlags).c_str(), queue.queueCount,
              util::getBoolStr(presentationSupported == VK_TRUE));

    VulkanQueues::QueueInfo info {
        .family   = n,
        .count    = queue.queueCount,
        .graphics = (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0,
        .compute  = (queue.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0,
        .transfer = (queue.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0,
        .present  = (bool)presentationSupported,
    };

    queueInfos.push_back(info);
  }

  // Prio
  auto gather = [&queueInfos, &qs](uint32_t count, std::vector<VulkanQueues::QueueInfo>& queue, auto const cmp) {
    for (uint32_t i = queue.size(); i < count; i++) {
      if (auto it = std::find_if(queueInfos.begin(), queueInfos.end(), cmp); it != queueInfos.end()) {
        it->index = 0;

        auto& item = queue.emplace_back(*it);
        if (it->count > qs.familyUsed[it->family]) {
          item.index = qs.familyUsed[it->family];
          qs.familyUsed[it->family]++;
        }

      } else {
        break;
      }
    }
  };

  gather(1, qs.graphics, [](VulkanQueues::QueueInfo& q) { return q.graphics && q.transfer && q.present; });
  gather(1, qs.present, [](VulkanQueues::QueueInfo& q) { return q.graphics && q.transfer && q.present; });
  gather(1, qs.transfer, [](VulkanQueues::QueueInfo& q) { return q.transfer && !q.graphics && !q.compute; });
  gather(1, qs.compute, [](VulkanQueues::QueueInfo& q) { return q.compute && !q.graphics; });
  //-

  // Fill in whatever is left
  gather(1, qs.graphics, [](VulkanQueues::QueueInfo& q) { return q.transfer == true; });
  gather(1, qs.present, [](VulkanQueues::QueueInfo& q) { return q.present == true; });
  gather(1, qs.transfer, [](VulkanQueues::QueueInfo& q) { return q.graphics == true; });
  gather(1, qs.compute, [](VulkanQueues::QueueInfo& q) { return q.compute == true; });
  //

  //-

  return qs;
}

void dumpQueues(const VulkanQueues& qs) {
  LOG_USE_MODULE(vulkanSetup);

  std::string familyString;
  for (auto u: qs.familyUsed) {
    familyString += std::to_string(u) + std::string(", ");
  }
  LOG_INFO(L"\t familyUsed = [%S]", familyString.c_str());

  LOG_INFO(L"\t graphics:");
  for (const auto& q: qs.graphics) {
    LOG_INFO(L"\t\t family:%u index:%u", q.family, q.index);
  }

  LOG_INFO(L"\t compute:");
  for (const auto& q: qs.compute) {
    LOG_INFO(L"\t\t family:%u index:%u", q.family, q.index);
  }

  LOG_INFO(L"\t transfer:");
  for (const auto& q: qs.transfer) {
    LOG_INFO(L"\t\t family:%u, index:%u", q.family, q.index);
  }
  LOG_INFO(L"\t present:");
  for (const auto& q: qs.present) {
    LOG_INFO(L"\t\t family:%u, index:%u", q.family, q.index);
  }
}

void getSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, SurfaceCapabilities& r) {
  LOG_USE_MODULE(vulkanSetup);

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &r.capabilities);

  uint32_t formats_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formats_count, nullptr);

  r.formats = std::vector<VkSurfaceFormatKHR>(formats_count, VkSurfaceFormatKHR {});
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formats_count, r.formats.data());

  uint32_t present_modes_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_modes_count, nullptr);

  r.presentModes = std::vector<VkPresentModeKHR>(present_modes_count, VkPresentModeKHR {});
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_modes_count, r.presentModes.data());

  r.format_srgb_bgra32 = false;
  for (const auto& f: r.formats) {
    if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      r.format_srgb_bgra32 = true;
      break;
    }
    if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      r.format_unorm_bgra32 = true;
      break;
    }
  }
}

bool checkFormat(VkPhysicalDevice device, VkFormat format, VkFormatFeatureFlags features) {
  VkFormatProperties formatProps;
  vkGetPhysicalDeviceFormatProperties(device, format, &formatProps);

  if ((formatProps.optimalTilingFeatures & features) == features || (formatProps.linearTilingFeatures & features) == features) {
    return true;
  }
  return false;
}

void findPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, SurfaceCapabilities* outCapabilities, VkPhysicalDevice* outDevice, VulkanQueues* outQueues,
                        bool enableValidation) {
  LOG_USE_MODULE(vulkanSetup);

  uint32_t devicesCount = 0;
  vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);

  if (devicesCount == 0) {
    LOG_CRIT(L"No GPUs found");
  }

  std::vector<VkPhysicalDevice> devices(devicesCount);
  vkEnumeratePhysicalDevices(instance, &devicesCount, devices.data());

  // Find Best
  VkPhysicalDevice bestDevice = nullptr;
  VulkanQueues     bestQueues;

  bool skipDevice = false;
  for (const auto& device: devices) {
    skipDevice = false;

    VkPhysicalDeviceInlineUniformBlockFeaturesEXT uniBlock {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES,
        .pNext = nullptr,
    };

    VkPhysicalDeviceExtendedDynamicState3FeaturesEXT extendedDynamic3Features {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT,
        .pNext = &uniBlock,
    };

    VkPhysicalDeviceDepthClipControlFeaturesEXT depthClipControlFeatures {
        .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT,
        .pNext            = &extendedDynamic3Features,
        .depthClipControl = VK_TRUE,
    };

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature {
        .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
        .pNext            = &depthClipControlFeatures,
        .dynamicRendering = VK_TRUE,
    };

    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeature {
        .sType             = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
        .pNext             = &dynamicRenderingFeature,
        .timelineSemaphore = VK_TRUE,
    };

    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddress {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
                                                                     .pNext = &timelineSemaphoreFeature};

    VkPhysicalDeviceDescriptorIndexingFeatures descIndexing {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .pNext = &bufferDeviceAddress,
    };

    VkPhysicalDeviceShaderObjectFeaturesEXT shaderObj {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT, .pNext = &descIndexing};

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeature {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT,
                                                                .pNext = &shaderObj};

    VkPhysicalDeviceColorWriteEnableFeaturesEXT colorWriteExt {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT,
        .pNext = &indexingFeature,
    };

    VkPhysicalDeviceFeatures2 deviceFeatures {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &colorWriteExt};

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures2(device, &deviceFeatures);

    LOG_INFO(L"Vulkan device: %S api:%u.%u:%u", deviceProperties.deviceName, VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
             VK_API_VERSION_MINOR(deviceProperties.apiVersion), VK_API_VERSION_PATCH(deviceProperties.apiVersion));

    auto qs = findQueues(device, surface);
    dumpQueues(qs);

    if (shaderObj.shaderObject == VK_FALSE) {
      LOG_ERR(L"shaderObject is not supported");
      skipDevice = true;
    }

    if (qs.graphics.empty() || qs.compute.empty() || qs.transfer.empty() || qs.present.empty()) {
      LOG_ERR(L"Not enough queues");
      skipDevice = true;
    }

    if (colorWriteExt.colorWriteEnable != VK_TRUE) {
      LOG_ERR(L"colorWriteEnable is not supported");
      skipDevice = true;
    }

    if (deviceFeatures.features.fragmentStoresAndAtomics != VK_TRUE) {
      LOG_ERR(L"fragmentStoresAndAtomics is not supported");
      skipDevice = true;
    }

    if (deviceFeatures.features.samplerAnisotropy != VK_TRUE) {
      LOG_ERR(L"samplerAnisotropy is not supported");
      skipDevice = true;
    }

    if (!skipDevice) {
      uint32_t numExt = 0;
      vkEnumerateDeviceExtensionProperties(device, nullptr, &numExt, nullptr);
      if (numExt == 0) {
        LOG_CRIT(L"no extensions found");
      }

      std::vector<VkExtensionProperties> availableExt(numExt, VkExtensionProperties {});
      vkEnumerateDeviceExtensionProperties(device, nullptr, &numExt, availableExt.data());

      for (const char* ext: requiredExtensions) {
        if (std::find_if(availableExt.begin(), availableExt.end(), [&ext](auto p) { return strcmp(p.extensionName, ext) == 0; }) == availableExt.end()) {
          LOG_ERR(L"%S not supported", ext);
          skipDevice = true;
          break;
        }
      }
      if (enableValidation) {
        for (const auto& ext: availableExt) {
          LOG_DEBUG(L"Vulkan available extension: %S, version:%u", ext.extensionName, ext.specVersion);
        }
      }
    }
    if (!skipDevice) {
      getSurfaceCapabilities(device, surface, *outCapabilities);

      if ((outCapabilities->capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0) {
        LOG_DEBUG(L"Surface cannot be destination of blit");
        skipDevice = true;
      }
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_R8G8B8A8_SRGB cannot be used as transfer source");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_D32_SFLOAT, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_D32_SFLOAT cannot be used as depth buffer");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_D32_SFLOAT_S8_UINT cannot be used as depth buffer");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_D16_UNORM, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_D16_UNORM cannot be used as depth buffer");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_D24_UNORM_S8_UINT cannot be used as depth buffer");
      // skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_BC3_SRGB_BLOCK, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_BC3_SRGB_BLOCK cannot be used as texture");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_R8G8B8A8_SRGB cannot be used as texture");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_R8_UNORM, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_R8_UNORM cannot be used as texture");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_R8G8_UNORM, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_R8G8_UNORM cannot be used as texture");
      skipDevice = true;
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_R8G8B8A8_SRGB cannot be used as texture");

      if (!skipDevice && !checkFormat(device, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
        LOG_DEBUG(L"Format VK_FORMAT_R8G8B8A8_UNORM cannot be used as texture");
        skipDevice = true;
      }
    }

    if (!skipDevice && !checkFormat(device, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
      LOG_DEBUG(L"Format VK_FORMAT_B8G8R8A8_SRGB cannot be used as texture");

      if (!skipDevice && !checkFormat(device, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT)) {
        LOG_DEBUG(L"Format VK_FORMAT_B8G8R8A8_UNORM cannot be used as texture");
        skipDevice = true;
      }
    }

    if (!skipDevice && deviceProperties.limits.maxSamplerAnisotropy < 16.0f) {
      LOG_DEBUG(L"maxSamplerAnisotropy < 16.0f");
      skipDevice = true;
    }

    if (skipDevice) {
      continue;
    }

    if (bestDevice == nullptr || deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      bestDevice = device;
      bestQueues = qs;
    }
  }

  // -Find best

  *outDevice = bestDevice;
  *outQueues = bestQueues;
}

VkDevice createDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanExtensions const extensions, VulkanQueues const& queues,
                      bool enableValidation) {
  LOG_USE_MODULE(vulkanSetup);
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfo(queues.familyCount);
  std::vector<std::vector<float>>      queuePrio(queues.familyCount);

  uint32_t numQueueCreateInfo = 0;
  for (uint32_t i = 0; i < queues.familyCount; i++) {
    if (queues.familyUsed[i] != 0) {
      for (uint32_t pi = 0; pi < queues.familyUsed[i]; pi++) {
        queuePrio[numQueueCreateInfo].push_back(1.0f);
      }

      queueCreateInfo[numQueueCreateInfo].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo[numQueueCreateInfo].pNext            = nullptr;
      queueCreateInfo[numQueueCreateInfo].flags            = 0;
      queueCreateInfo[numQueueCreateInfo].queueFamilyIndex = i;
      queueCreateInfo[numQueueCreateInfo].queueCount       = queues.familyUsed[i];
      queueCreateInfo[numQueueCreateInfo].pQueuePriorities = queuePrio[numQueueCreateInfo].data();

      numQueueCreateInfo++;
    }
  }

  VkPhysicalDeviceFeatures deviceFeatures {
      .geometryShader = VK_TRUE, .fillModeNonSolid = VK_TRUE, .wideLines = VK_TRUE, .samplerAnisotropy = VK_TRUE, .fragmentStoresAndAtomics = VK_TRUE,
      // deviceFeatures.shaderImageGatherExtended = VK_TRUE;
  };

  VkPhysicalDeviceInlineUniformBlockFeaturesEXT uniBlock {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES, .pNext = nullptr,
      //.inlineUniformBlock                                 = VK_TRUE,
      //.descriptorBindingInlineUniformBlockUpdateAfterBind = VK_TRUE,
  };

  VkPhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT, .shaderObject = VK_TRUE};

  VkPhysicalDeviceExtendedDynamicState3FeaturesEXT extendedDynamic3Features {
      .sType                                      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT,
      .pNext                                      = &uniBlock,
      .extendedDynamicState3PolygonMode           = VK_TRUE,
      .extendedDynamicState3RasterizationSamples  = VK_TRUE,
      .extendedDynamicState3AlphaToCoverageEnable = VK_TRUE,
      //.extendedDynamicState3AlphaToOneEnable          = VK_TRUE,
      .extendedDynamicState3ColorBlendEnable          = VK_TRUE,
      .extendedDynamicState3ColorBlendEquation        = VK_TRUE,
      .extendedDynamicState3ColorWriteMask            = VK_TRUE,
      .extendedDynamicState3DepthClipNegativeOneToOne = VK_TRUE,
  };

  VkPhysicalDeviceDepthClipControlFeaturesEXT depthClipControlFeatures {
      .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT,
      .pNext            = &extendedDynamic3Features,
      .depthClipControl = VK_TRUE,
  };

  VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature {
      .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
      .pNext            = &depthClipControlFeatures,
      .dynamicRendering = VK_TRUE,
  };

  VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeature {
      .sType             = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
      .pNext             = &dynamicRenderingFeature,
      .timelineSemaphore = VK_TRUE,
  };

  VkPhysicalDeviceColorWriteEnableFeaturesEXT colorWriteExt {
      .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT,
      .pNext            = &timelineSemaphoreFeature,
      .colorWriteEnable = VK_TRUE,
  };

  VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddress {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES, .pNext = &colorWriteExt, .bufferDeviceAddress = VK_TRUE,

      //.bufferDeviceAddressCaptureReplay = enableValidation ? VK_TRUE : VK_FALSE,
  };

  VkPhysicalDeviceDescriptorIndexingFeatures const descIndexing {
      .sType                                         = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
      .pNext                                         = &bufferDeviceAddress,
      .descriptorBindingUniformBufferUpdateAfterBind = VK_FALSE, // Todo: only optional!
      .descriptorBindingSampledImageUpdateAfterBind  = VK_TRUE,
      .descriptorBindingStorageImageUpdateAfterBind  = VK_TRUE,
      .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
      .descriptorBindingPartiallyBound               = VK_TRUE,
      .descriptorBindingVariableDescriptorCount      = VK_TRUE,
      .runtimeDescriptorArray                        = VK_TRUE,
  };

  VkDeviceCreateInfo const createInfo {
      .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext                   = &descIndexing,
      .flags                   = 0,
      .queueCreateInfoCount    = numQueueCreateInfo,
      .pQueueCreateInfos       = queueCreateInfo.data(),
      .enabledLayerCount       = (uint32_t)extensions.requiredLayers.size(),
      .ppEnabledLayerNames     = extensions.requiredLayers.data(),
      .enabledExtensionCount   = requiredExtensions.size(),
      .ppEnabledExtensionNames = requiredExtensions.data(),
      .pEnabledFeatures        = &deviceFeatures,
  };

  VkDevice device = nullptr;
  if (auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device); result != VK_SUCCESS) {
    LOG_CRIT(L"Couldn't create vulkanDevice %S", string_VkResult(result));
  }

  return device;
}

static VkPhysicalDeviceProperties g_PhysicalDeviceProperties;
static VkInstance                 g_VkInstance;

VulkanObj* initVulkan(SDL_Window* window, VkSurfaceKHR& surface, bool enableValidation) {
  LOG_USE_MODULE(vulkanSetup);
  auto obj = new VulkanObj;

  // Create Instance&Debug
  VkDebugUtilsMessengerCreateInfoEXT dbgCreateInfo {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity =
          static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) | static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) |
          static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) | static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
      .messageType = static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) |
                     static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) |
                     static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
      .pfnUserCallback = debugMessengerCallback,
      .pUserData       = nullptr,
  };

  VkApplicationInfo const appInfo {
      .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext              = nullptr,
      .pApplicationName   = "psOff",
      .applicationVersion = 1,
      .pEngineName        = "psEngine",
      .engineVersion      = 1,
      .apiVersion         = VK_API_VERSION_1_3,
  };

  auto extensions = getExtensions(window, enableValidation);

  VkInstanceCreateInfo const instInfo {
      .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext                   = extensions.enableValidationLayers ? &dbgCreateInfo : nullptr,
      .flags                   = 0,
      .pApplicationInfo        = &appInfo,
      .enabledLayerCount       = (uint32_t)extensions.requiredLayers.size(),
      .ppEnabledLayerNames     = extensions.requiredLayers.data(),
      .enabledExtensionCount   = (uint32_t)extensions.requiredExtensions.size(),
      .ppEnabledExtensionNames = extensions.requiredExtensions.data(),
  };

  if (VkResult result = vkCreateInstance(&instInfo, nullptr, &obj->deviceInfo.instance); result != VK_SUCCESS) {
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
      LOG_CRIT(L"vkCreateInstance() Error:VK_ERROR_INCOMPATIBLE_DRIVER");
    else
      LOG_CRIT(L"vkCreateInstance() Error:%S", string_VkResult(result));
  }
  g_VkInstance = obj->deviceInfo.instance;

  if (extensions.enableValidationLayers) {
    // dbgCreateInfo.pNext = nullptr;
    if (auto result = createDebugUtilsMessengerEXT(obj->deviceInfo.instance, &dbgCreateInfo, nullptr, &obj->debugMessenger); result != VK_SUCCESS) {
      LOG_CRIT(L"createDebugUtilsMessengerEXT() %S", string_VkResult(result));
    }
  }
  // -

  if (auto result = SDL_Vulkan_CreateSurface(window, obj->deviceInfo.instance, &surface); result != true) {
    LOG_CRIT(L"SDL_Vulkan_CreateSurface() Error:%S", SDL_GetError());
  }

  VulkanQueues queues;
  findPhysicalDevice(obj->deviceInfo.instance, surface, &obj->surfaceCapabilities, &obj->deviceInfo.physicalDevice, &queues, enableValidation);
  if (obj->deviceInfo.physicalDevice == nullptr) {
    LOG_CRIT(L"Couldn't find a suitable device");
  }

  vkGetPhysicalDeviceProperties(obj->deviceInfo.physicalDevice, &g_PhysicalDeviceProperties);

  {
    auto const text =
        std::format("Selected GPU:{} api:{}.{}.{}", g_PhysicalDeviceProperties.deviceName, VK_API_VERSION_MAJOR(g_PhysicalDeviceProperties.apiVersion),
                    VK_API_VERSION_MINOR(g_PhysicalDeviceProperties.apiVersion), VK_API_VERSION_PATCH(g_PhysicalDeviceProperties.apiVersion));
    printf("%s\n", text.data());
    LOG_INFO(L"%S", text.data());
  }

  obj->deviceInfo.device = createDevice(obj->deviceInfo.physicalDevice, surface, extensions, queues, enableValidation);

  // Create queues
  {
    auto queueFunc = [](VkDevice device, std::vector<VulkanQueues::QueueInfo> const queueInfos, std::vector<std::unique_ptr<QueueInfo>>& out) {
      for (auto& item: queueInfos) {
        VkQueue queue;
        vkGetDeviceQueue(device, item.family, item.index, &queue);
        out.push_back(std::make_unique<QueueInfo>(queue, item.family));
      }
    };

    queueFunc(obj->deviceInfo.device, queues.compute, obj->queues.items[getIndex(QueueType::compute)]);
    queueFunc(obj->deviceInfo.device, queues.present, obj->queues.items[getIndex(QueueType::present)]);
    queueFunc(obj->deviceInfo.device, queues.transfer, obj->queues.items[getIndex(QueueType::transfer)]);
    queueFunc(obj->deviceInfo.device, queues.graphics, obj->queues.items[getIndex(QueueType::graphics)]);
  }
  //-
  return obj;
}

VkPhysicalDeviceLimits const* getPhysicalLimits() {
  return &g_PhysicalDeviceProperties.limits;
}

VkInstance const getVkInstance() {
  return g_VkInstance;
}

std::string_view const getGPUName() {
  return g_PhysicalDeviceProperties.deviceName;
}

void deinitVulkan(VulkanObj* obj) {
  vkDestroyInstance(obj->deviceInfo.instance, nullptr);
  vkDestroyDevice(obj->deviceInfo.device, nullptr);

  delete obj;
}
} // namespace vulkan
