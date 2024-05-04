add_library(videoout OBJECT
  videoout.cpp
  imageHandler.cpp

  vulkan/vulkanSetup.cpp
  vulkan/vulkanHelper.cpp

  overlay/overlay.cpp
)

add_dependencies(videoout third_party psOff_utility gamereport initParams config_emu psoff_render)

target_include_directories(videoout PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/include
)