add_library(videoout OBJECT
  videoout.cpp
  vulkan/vulkanSetup.cpp
  vulkan/vulkanHelper.cpp
)

target_include_directories(videoout PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/third_party/optick/src
)