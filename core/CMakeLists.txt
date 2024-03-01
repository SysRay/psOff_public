# Place before add_subdir -> define for all childs
add_compile_definitions(
  BOOST_ALL_NO_LIB
)

# Add directories
add_subdirectory(kernel)
add_subdirectory(videoout)
add_subdirectory(initParams)
add_subdirectory(timer)
add_subdirectory(systemContent)
add_subdirectory(fileManager)

# Build
add_library(core SHARED
  $<TARGET_OBJECTS:kernel>
  $<TARGET_OBJECTS:initParams>
  $<TARGET_OBJECTS:timer>
  $<TARGET_OBJECTS:systemContent>
  $<TARGET_OBJECTS:videoout>
  $<TARGET_OBJECTS:fileManager>
)

add_dependencies(core logging emuExports)
target_link_libraries(core PRIVATE
  libboost_thread
  libboost_chrono
  libboost_program_options
  glfw3
  OptickCore
  emulator.lib
  psOff_utility
  ${Vulkan_LIBRARIES}
)