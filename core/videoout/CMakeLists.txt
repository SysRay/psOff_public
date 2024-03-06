add_library(videoout OBJECT
  videoout.cpp
  vulkan/vulkanSetup.cpp
  vulkan/vulkanHelper.cpp
)

add_dependencies(videoout third_party psOff_utility initParams imports)
target_include_directories(videoout PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/third_party/optick/src
  ${PRJ_SRC_DIR}/third_party/magic_enum/include
)