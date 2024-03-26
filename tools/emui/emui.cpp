#include "emui.h"

#include "core/videoout/emuictx.h"

EmUI::~EmUI() {

}

void EmUI::init() {
  EmUICtx_out data;
  // acessEmUIContext().getImGui(&data);
  ImGui::SetCurrentContext(data.ctx); // Should be called inside emui.dll to set context from core.dll
  ImGui::SetAllocatorFunctions(data.allocFunc, data.freeFunc, data.userData); // Same thing as above
}

void EmUI::draw() { // Should be called after `ImGui_ImplVulkan_NewFrame();` and `ImGui_ImplSDL2_NewFrame();`
  static bool show_demo = true;

  ImGui::NewFrame();

  ImGui::ShowDemoWindow(&show_demo); // Show ImGui demo window

  ImGui::EndFrame();
}
