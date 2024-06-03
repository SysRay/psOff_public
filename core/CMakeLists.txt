# Place before add_subdir -> define for all childs
add_compile_definitions(
  BOOST_ALL_NO_LIB
)

set(DST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/core/export/include/core/")

# Add directories
add_subdirectory(kernel)
add_subdirectory(videoout)
add_subdirectory(initParams)
add_subdirectory(timer)
add_subdirectory(systemContent)
add_subdirectory(networking)
add_subdirectory(hotkeys)
add_subdirectory(trophies)
add_subdirectory(fileManager)
add_subdirectory(memory)
add_subdirectory(dmem)
add_subdirectory(unwinding)
add_subdirectory(runtime)

# Build
add_library(core SHARED
  $<TARGET_OBJECTS:kernel>
  $<TARGET_OBJECTS:initParams>
  $<TARGET_OBJECTS:timer>
  $<TARGET_OBJECTS:systemContent>
  $<TARGET_OBJECTS:networking>
  $<TARGET_OBJECTS:hotkeys>
  $<TARGET_OBJECTS:trophies>
  $<TARGET_OBJECTS:videoout>
  $<TARGET_OBJECTS:fileManager>
  $<TARGET_OBJECTS:memory>
  $<TARGET_OBJECTS:dmem>
  $<TARGET_OBJECTS:unwinding>
  $<TARGET_OBJECTS:runtime>
)

add_dependencies(core logging third_party config_emu psoff_render)
target_link_libraries(core PRIVATE
  libboost_url
  libboost_thread
  libboost_chrono
  libboost_program_options
  libboost_filesystem
  sdl2
  psoff_render.lib
  OptickCore
  psOff_utility
  config_emu.lib
  onecore.lib
  IPHLPAPI.lib
  Dbghelp.lib
  wepoll.lib
  Ws2_32.lib
  libcrypto.lib
  libssl.lib
  pugixml.lib
  ntdll.dll
  imgui
  VulkanMemoryAllocator
  ${Vulkan_LIBRARIES}
)

install(FILES $<TARGET_PDB_FILE:core> DESTINATION debug OPTIONAL)
install(TARGETS core LIBRARY DESTINATION .)

# Manage symbols
if(NOT ISDEBUG)
  add_custom_command(
    TARGET core POST_BUILD
    COMMAND $<$<CONFIG:release>:${CMAKE_STRIP}>
    ARGS --strip-all $<TARGET_FILE:core>
  )
endif()

set_target_properties(core
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/core/export/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/core/export/bin"
  PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/"
)

file(COPY imports/exports/pm4_custom.h DESTINATION ${DST_INCLUDE_DIR}/imports/exports)