#pragma once

#include <vulkan/vulkan_core.h>

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif

/**
 * @brief Get the required memory alignment. Dont' call from constructor.
 *
 * @param format
 * @param extent shouldn't matter if not 100% correct
 * @return memory alignment
 */

__APICALL uint64_t getImageAlignment(VkFormat format, VkExtent3D const& extent);
#undef __APICALL