#define __APICALL_EXTERN
#include "imports.h"

#include "imports_func.h"

#undef __APICALL_EXTERN

#include "utility/utility.h"

#include <assert.h>

// todo: Change after splitting the project
static getImageAlignment_t     g_getImageAlignment     = nullptr;
static registerDisplayBuffer_t g_registerDisplayBuffer = nullptr;
static getDisplayBuffer_t      g_getDisplayBuffer      = nullptr;
static createGraphics_t        g_createGraphics        = nullptr;

uint64_t getImageAlignment(VkFormat format, VkExtent3D extent) {
  assert(g_getImageAlignment != nullptr);
  return g_getImageAlignment(format, extent);
}

bool registerDisplayBuffer(uint64_t vaddr, VkExtent2D extent, uint32_t pitch, VkFormat format) {
  assert(g_registerDisplayBuffer != nullptr);
  return g_registerDisplayBuffer(vaddr, extent, pitch, format);
}

std::shared_ptr<IGpuImageObject> getDisplayBuffer(uint64_t vaddr) {
  assert(g_getDisplayBuffer != nullptr);
  return g_getDisplayBuffer(vaddr);
}

std::unique_ptr<IGraphics> createGraphics(IEventsGraphics& listener, VkDevice device, VkPhysicalDevice physDev, VkInstance instance) {
  assert(g_createGraphics != nullptr);
  return g_createGraphics(listener, device, physDev, instance);
}

void setCallback_getImageAlignment(getImageAlignment_t cb) {
  g_getImageAlignment = cb;
}

void setCallback_registerDisplayBuffer(registerDisplayBuffer_t cb) {
  g_registerDisplayBuffer = cb;
}

void setCallback_getDisplayBuffer(getDisplayBuffer_t cb) {
  g_getDisplayBuffer = cb;
}

void setCallback_createGraphics(createGraphics_t cb) {
  g_createGraphics = cb;
}
