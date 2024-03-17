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

// All communication  strictly goes through the core library. (May change later)

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
 * @brief Copy the displaybuffer to swapchain-buffer
 *
 * @param vaddr addr of displaybuffer
 * @param transferBuffer copy commands are added here
 * @return true
 * @return false
 */
__APICALL bool copyDisplayBuffer(uint64_t vaddr, VkCommandBuffer transferBuffer, VkImage dstImage);

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