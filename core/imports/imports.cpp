#define __APICALL_EXTERN
#include "imports.h"

#include "imports_func.h"
#include "imports_gpuMemory.h"
#include "imports_runtime.h"

#undef __APICALL_EXTERN
#include "core/imports/exports/runtimeExport.h"
#include "utility/utility.h"

#include <assert.h>

// Change if called from construct is needed
static registerDisplayBuffer_t g_registerDisplayBuffer = nullptr;
static copyDisplayBuffer_t     g_copyDisplayBuffer     = nullptr;
static createGraphics_t        g_createGraphics        = nullptr;
static runtimeExport_t         g_runtimeExport         = nullptr;

static notify_allocHeap_t g_notify_allocHeap = nullptr;
static isGPULocal_t       g_isGPULocal       = nullptr;

bool registerDisplayBuffer(uint64_t vaddr, VkExtent2D extent, uint32_t pitch, VkFormat format) {
  assert(g_registerDisplayBuffer != nullptr);
  return g_registerDisplayBuffer(vaddr, extent, pitch, format);
}

bool copyDisplayBuffer(uint64_t vaddr, VkCommandBuffer transferBuffer, VkImage dstImage, VkExtent2D dstExtent) {
  assert(g_copyDisplayBuffer != nullptr);
  return g_copyDisplayBuffer(vaddr, transferBuffer, dstImage, dstExtent);
}

std::unique_ptr<IGraphics> createGraphics(IEventsGraphics& listener, VkDevice device, VkPhysicalDevice physDev, VkInstance instance) {
  assert(g_createGraphics != nullptr);
  return g_createGraphics(listener, device, physDev, instance);
}

IRuntimeExport* accessRuntimeExport() {
  return g_runtimeExport;
}

bool gpuMemory::notify_allocHeap(uint64_t vaddr, uint64_t size, int memoryProtection) {
  return g_notify_allocHeap(vaddr, size, memoryProtection);
}

bool gpuMemory::isGPULocal(uint64_t vaddr) {
  return g_isGPULocal(vaddr);
}

void setCallback_registerDisplayBuffer(registerDisplayBuffer_t cb) {
  g_registerDisplayBuffer = cb;
}

void setCallback_copyDisplayBuffer(copyDisplayBuffer_t cb) {
  g_copyDisplayBuffer = cb;
}

void setCallback_createGraphics(createGraphics_t cb) {
  g_createGraphics = cb;
}

void setCallback_accessRuntimeExport(runtimeExport_t cb) {
  g_runtimeExport = cb;
}

void setCallback_notify_allocHeap(notify_allocHeap_t cb) {
  g_notify_allocHeap = cb;
}

void setCallback_isGPULocal(isGPULocal_t cb) {
  g_isGPULocal = cb;
}