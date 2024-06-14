cmake_minimum_required(VERSION 3.24)

add_library(logging SHARED logging.cpp)

add_dependencies(logging third_party config_emu)

target_link_libraries(logging PRIVATE ws2_32 p7 config_emu.lib)

target_compile_options(logging PRIVATE "/Zi")

set_target_properties(logging
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Stub
add_library(logging_stub SHARED logging_stub.cpp)

set_target_properties(logging_stub
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)