cmake_minimum_required(VERSION 3.24)
project(third_party VERSION 0.0.1)

include(ExternalProject)

add_subdirectory(optick)
add_subdirectory(p7)
add_subdirectory(SDL2)
add_subdirectory(libpng)

install(DIRECTORY "ffmpeg/bin/" DESTINATION "${CMAKE_BINARY_DIR}/bin"
  FILES_MATCHING PATTERN "*.dll"
)

set_target_properties(OptickCore
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/lib"
)

set_target_properties(SDL2
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/bin"
)

set_target_properties(libpng
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/bin"
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(PNG_SHARED ON CACHE BOOL "" FORCE)
set(PNG_STATIC OFF CACHE BOOL "" FORCE)
set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)
