add_library(videoout OBJECT
  videoout.cpp
  vulkan/vulkanSetup.cpp
  vulkan/vulkanHelper.cpp
  imageHandler.cpp
)

add_dependencies(videoout third_party psOff_utility gamereport initParams config_emu psoff_render)

target_include_directories(videoout PRIVATE
  ${Vulkan_INCLUDE_DIRS}
)
