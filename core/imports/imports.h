#pragma once

#include "exports/gpuMemory_types.h"
#include "exports/graphics.h"

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

using registerDisplayBuffer_t = bool (*)(uint64_t vaddr, VkExtent2D extent, uint32_t pitch, VkFormat format);

using getDisplayBuffer_t = std::shared_ptr<IGpuImageObject> (*)(uint64_t vaddr);

using createGraphics_t = std::unique_ptr<IGraphics> (*)(IEventsGraphics& listener, VkDevice device, VkPhysicalDevice physDev, VkInstance instance);

using runtimeExport_t = class IRuntimeExport*;

using notify_allocHeap_t = bool (*)(uint64_t vaddr, uint64_t size, int memoryProtection);

using isGPULocal_t = bool (*)(uint64_t vaddr);

__APICALL void setCallback_registerDisplayBuffer(registerDisplayBuffer_t);

__APICALL void setCallback_getDisplayBuffer(getDisplayBuffer_t);

__APICALL void setCallback_createGraphics(createGraphics_t);

__APICALL void setCallback_accessRuntimeExport(runtimeExport_t);
__APICALL void setCallback_notify_allocHeap(notify_allocHeap_t);
__APICALL void setCallback_isGPULocal(isGPULocal_t);

#undef __APICALL