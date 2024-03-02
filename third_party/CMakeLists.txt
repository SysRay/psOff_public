cmake_minimum_required(VERSION 3.24)
project(third_party VERSION 0.0.1)

include(ExternalProject)

add_subdirectory(portAudio)
add_subdirectory(optick)
add_subdirectory(p7)

install(DIRECTORY "ffmpeg/bin/" DESTINATION "${CMAKE_BINARY_DIR}/bin"
  FILES_MATCHING PATTERN "*.dll"
)

set_target_properties(OptickCore
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/lib"
)

# Boost
set(BOOST_INCLUDE_LIBRARIES "program_options;date_time;interprocess;stacktrace;uuid;beast;signals2;thread")
ExternalProject_Add(boost_thirdParty
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boost
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/boost
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=Release
  -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/install
  -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
  -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
  -DBoost_USE_STATIC_LIBS=ON
  -DBoost_USE_MULTITHREADED=ON
  -DBUILD_TESTING=OFF
  -DBOOST_INSTALL_LAYOUT=system
  CMAKE_CACHE_ARGS -DBOOST_INCLUDE_LIBRARIES:STRING=${BOOST_INCLUDE_LIBRARIES}
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" ON)
option(GLFW_VULKAN_STATIC "Assume the Vulkan loader is linked with the application" ON)
add_subdirectory(glfw)