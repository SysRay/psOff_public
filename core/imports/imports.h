#pragma once

#include <gpuMemory_types.h>
#include <graphics.h>
#include <memory>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

using getImageAlignment_t = uint64_t (*)(VkFormat format, VkExtent3D extent);

using registerDisplayBuffer_t = bool (*)(uint64_t vaddr, VkExtent2D extent, uint32_t pitch, VkFormat format);

using getDisplayBuffer_t = std::shared_ptr<IGpuImageObject> (*)(uint64_t vaddr);

using createGraphics_t = std::unique_ptr<IGraphics> (*)(IEventsGraphics& listener, VkDevice device, VkPhysicalDevice physDev, VkInstance instance);

using runtimeExport_t = class IRuntimeExport*;

__APICALL void setCallback_getImageAlignment(getImageAlignment_t);

__APICALL void setCallback_registerDisplayBuffer(registerDisplayBuffer_t);

__APICALL void setCallback_getDisplayBuffer(getDisplayBuffer_t);

__APICALL void setCallback_createGraphics(createGraphics_t);

__APICALL void setCallback_accessRuntimeExport(runtimeExport_t);

#undef __APICALL