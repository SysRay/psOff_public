#pragma once

#include <imgui/imgui.h>

#define TR_BRONZE_COLOR   IM_COL32(199, 124, 48, 255)
#define TR_SILVER_COLOR   IM_COL32(150, 164, 182, 255)
#define TR_GOLD_COLOR     IM_COL32(228, 194, 78, 255)
#define TR_PLATINUM_COLOR IM_COL32(191, 191, 191, 255)
#define TR_UNKNOWN_COLOR  TR_BRONZE_COLOR

static inline ImFont* _ImGuiCreateFont(float px) {
  ImGuiIO&     io = ImGui::GetIO();
  ImFontConfig cfg;
  cfg.MergeMode            = true;
  cfg.FontDataOwnedByAtlas = false;

  ImVector<ImWchar>        ranges;
  ImFontGlyphRangesBuilder builder;
  builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
  builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
  builder.BuildRanges(&ranges);

  auto font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", px, nullptr, ranges.Data);
  io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Msyhl.ttc", px, &cfg, io.Fonts->GetGlyphRangesChineseFull());
  io.Fonts->Build();

  return font;
}
