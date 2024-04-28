#define __APICALL_EXTERN
#include "imports.h"

#include "imports_func.h"

#undef __APICALL_EXTERN
#include "utility/utility.h"

#include <assert.h>

// Change if called from construct is needed
static createGraphics_t g_createGraphics = nullptr;

static getEmulatorVersion_t g_getEmulatorVersion = nullptr;

std::unique_ptr<IGraphics> createGraphics(IEventsGraphics& listener, std::shared_ptr<vulkan::DeviceInfo>& deviceInfo) {
  assert(g_createGraphics != nullptr);
  return g_createGraphics(listener, deviceInfo);
}

std::string_view getEmulatorVersion() {
  assert(g_getEmulatorVersion != nullptr);
  return g_getEmulatorVersion();
}

void setCallback_createGraphics(createGraphics_t cb) {
  g_createGraphics = cb;
}

void setCallback_getEmulatorVersion(getEmulatorVersion_t cb) {
  g_getEmulatorVersion = cb;
}