add_library(videoout OBJECT
  videoout.cpp
  vulkan/vulkanSetup.cpp
  vulkan/vulkanHelper.cpp
  imageHandler.cpp
)

add_dependencies(videoout third_party psOff_utility gamereport initParams imports boost config_emu)
target_include_directories(videoout PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/third_party/optick/src
  ${PRJ_SRC_DIR}/third_party/magic_enum/include
  ${PRJ_SRC_DIR}/third_party/SDL2/include
)
