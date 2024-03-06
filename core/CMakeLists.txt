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
add_subdirectory(imports)
add_subdirectory(semaphore)
add_subdirectory(memory)
add_subdirectory(dmem)

# Build
add_library(core SHARED
  $<TARGET_OBJECTS:kernel>
  $<TARGET_OBJECTS:initParams>
  $<TARGET_OBJECTS:timer>
  $<TARGET_OBJECTS:systemContent>
  $<TARGET_OBJECTS:videoout>
  $<TARGET_OBJECTS:fileManager>
  $<TARGET_OBJECTS:imports>
  $<TARGET_OBJECTS:semaphore>
  $<TARGET_OBJECTS:memory>
  $<TARGET_OBJECTS:dmem>
)

add_dependencies(core logging)
target_link_libraries(core PRIVATE
  libboost_thread
  libboost_chrono
  libboost_program_options
  glfw3
  OptickCore
  psOff_utility
  ${Vulkan_LIBRARIES}
)

ADD_CUSTOM_COMMAND(TARGET core POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_BINARY_DIR}/core.lib
  ${CMAKE_BINARY_DIR}/lib)

  install(FILES $<TARGET_PDB_FILE:core> DESTINATION debug OPTIONAL)