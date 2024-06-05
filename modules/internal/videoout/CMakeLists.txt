add_library(videoout OBJECT
  videoout.cpp
  imageHandler.cpp

  vulkan/vulkanSetup.cpp
  vulkan/vulkanHelper.cpp

  overlay/overlay.cpp
  overlay/overtrophy/overtrophy.cpp

  # overlay/gamemenu/gamemenu.cpp
)

add_dependencies(videoout third_party psOff_utility initParams config_emu psoff_render)

target_include_directories(videoout PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/include
)

file(COPY videoout.h DESTINATION ${DST_INCLUDE_DIR}/videoout)
file(COPY vulkan/vulkanTypes.h DESTINATION ${DST_INCLUDE_DIR}/videoout/vulkan)