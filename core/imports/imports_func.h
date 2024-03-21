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