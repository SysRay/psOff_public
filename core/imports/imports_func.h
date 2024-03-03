#include "gpuMemory_types.h"

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

// All communication  strictly goes through the core library. (May change later)

/**
 * @brief Get the required memory alignment. Dont' call from constructor.
 *
 * @param format
 * @param extent shouldn't matter if not 100% correct
 * @return memory alignment
 */
__APICALL uint64_t getImageAlignment(VkFormat format, VkExtent3D extent);

/**
 * @brief register the displayBuffer for the GPU memory manager. Dont' call from constructor.
 *
 * @param vaddr
 * @param extent
 * @param pitch
 * @param format
 * @return true
 * @return false
 */
__APICALL bool registerDisplayBuffer(uint64_t vaddr, VkExtent2D extent, uint32_t pitch, VkFormat format);

/**
 * @brief Get the Display Buffer object. Dont' call from constructor.
 *
 * @param vaddr
 * @return std::shared_ptr<IGpuImageObject>
 */
__APICALL std::shared_ptr<IGpuImageObject> getDisplayBuffer(uint64_t vaddr);

/**
 * @brief Create a Graphics object.
 *
 * @param listener
 * @param device
 * @param physDev
 * @param instance
 * @return object
 */
__APICALL std::unique_ptr<IGraphics> createGraphics(IEventsGraphics& listener, VkDevice device, VkPhysicalDevice physDev, VkInstance instance);

#undef __APICALL