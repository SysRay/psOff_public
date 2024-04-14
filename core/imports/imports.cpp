#define __APICALL_EXTERN
#include "imports.h"

#include "imports_func.h"
#include "imports_runtime.h"

#undef __APICALL_EXTERN
#include "core/imports/exports/runtimeExport.h"
#include "utility/utility.h"

#include <assert.h>

// Change if called from construct is needed
static createGraphics_t g_createGraphics = nullptr;
static runtimeExport_t  g_runtimeExport  = nullptr;

static getEmulatorVersion_t g_getEmulatorVersion = nullptr;

std::unique_ptr<IGraphics> createGraphics(IEventsGraphics& listener, VkDevice device, VkPhysicalDevice physDev, VkInstance instance) {
  assert(g_createGraphics != nullptr);
  return g_createGraphics(listener, device, physDev, instance);
}

IRuntimeExport* accessRuntimeExport() {
  assert(g_runtimeExport != nullptr);
  return g_runtimeExport;
}

std::string_view getEmulatorVersion() {
  assert(g_getEmulatorVersion != nullptr);
  return g_getEmulatorVersion();
}

void setCallback_createGraphics(createGraphics_t cb) {
  g_createGraphics = cb;
}

void setCallback_accessRuntimeExport(runtimeExport_t cb) {
  g_runtimeExport = cb;
}

void setCallback_getEmulatorVersion(getEmulatorVersion_t cb) {
  g_getEmulatorVersion = cb;
}