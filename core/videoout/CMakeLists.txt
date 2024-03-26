set(IMGUI_ROOT ${PRJ_SRC_DIR}/third_party/imgui)

add_library(videoout OBJECT
  videoout.cpp
  vulkan/vulkanSetup.cpp
  vulkan/vulkanHelper.cpp

  # Overlay
  emuictx.cpp
  ${IMGUI_ROOT}/imgui.cpp
  ${IMGUI_ROOT}/imgui_draw.cpp
  ${IMGUI_ROOT}/imgui_tables.cpp
  ${IMGUI_ROOT}/imgui_widgets.cpp
  ${IMGUI_ROOT}/imgui_demo.cpp
  ${IMGUI_ROOT}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_ROOT}/backends/imgui_impl_vulkan.cpp
)

add_dependencies(videoout third_party psOff_utility initParams imports)
target_include_directories(videoout PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/third_party/optick/src
  ${PRJ_SRC_DIR}/third_party/magic_enum/include
  ${PRJ_SRC_DIR}/third_party/SDL2/include
  ${IMGUI_ROOT}/backends/
  ${IMGUI_ROOT}/
)
