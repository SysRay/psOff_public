#include "onscreenkbd.h"

#include "internal/oscctl/oscctl.h"

#include <imgui/imgui.h>

void OverKeyboard::draw() {
  if (auto params = accessOscCtl().getParams()) {
    if (params->status == IOscCtl::Status::SHOWN) {
      // ImGui::SetNextWindowPos(ImVec2(params->xpos, params->ypos));
      ImGui::SetNextWindowSize(ImVec2(0, 0));
      ImGui::Begin("Onscreen keyboard", nullptr, ImGuiWindowFlags_NoCollapse);

      ImGui::Text("%s", params->details.c_str());

      ImGui::InputText("EnteredText", params->internal_buffer, params->buffersz, ImGuiInputTextFlags_None /*, [](ImGuiInputTextCallbackData* data) -> int {}*/);

      if (ImGui::Button(params->enterLabel.c_str())) {
        accessOscCtl().enter();
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel")) {
        accessOscCtl().cancel();
      }

      ImGui::End();
    }
  }
}
