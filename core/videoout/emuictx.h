#pragma once

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_sdl2.h>

struct EmUICtx_out {
  ImGuiContext*      ctx;
  ImGuiMemAllocFunc  allocFunc;
  ImGuiMemFreeFunc   freeFunc;
  void*              userData;
};

class EmUICtx {
  EmUICtx_out m_data;

  public:
  EmUICtx() = default;
  virtual ~EmUICtx();

  VkDescriptorPool       m_dPool    = VK_NULL_HANDLE;
  VkAllocationCallbacks* m_allocCBs = nullptr;

  virtual void init();
  virtual void getImGui(EmUICtx_out*);

  void check(VkResult code);
};
