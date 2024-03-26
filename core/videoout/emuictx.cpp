#include "emuictx.h"

EmUICtx::~EmUICtx() {
  
}

void EmUICtx::init() {
  IMGUI_CHECKVERSION();
  m_data.ctx = ImGui::CreateContext(); // Saving created context to use it in emui.dll later
  ImGui::GetAllocatorFunctions(&m_data.allocFunc, &m_data.freeFunc, &m_data.userData); // Same thing
  ImGui::StyleColorsDark(); // Default ImGui theme
}

void EmUICtx::check(VkResult code) {
  // ???
}

void EmUICtx::getImGui(EmUICtx_out* out) {
  *out = m_data;
}
