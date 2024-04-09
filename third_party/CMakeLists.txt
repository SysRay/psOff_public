cmake_minimum_required(VERSION 3.24)
project(third_party VERSION 0.0.1)
include(ExternalProject)

add_subdirectory(optick)
add_subdirectory(p7)
add_subdirectory(SDL2)

set(gtest_force_shared_crt on)
add_subdirectory(googletest)

install(DIRECTORY "ffmpeg/bin/" DESTINATION "${CMAKE_BINARY_DIR}/bin"
  FILES_MATCHING PATTERN "*.dll"
)

install(DIRECTORY "openssl/bin/" DESTINATION "${CMAKE_BINARY_DIR}/bin"
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

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(PNG_SHARED ON CACHE BOOL "" FORCE)
set(PNG_STATIC OFF CACHE BOOL "" FORCE)
set(PNG_EXECUTABLES ON CACHE BOOL "" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)
set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/install/lib/zlib.lib)
set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/install/include)
add_subdirectory(libpng)
