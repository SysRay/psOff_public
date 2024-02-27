cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAudioOut)
project(${libName})

add_library(${libName} SHARED entry.cpp)
target_include_directories(${libName} PRIVATE
  ${CMAKE_SOURCE_DIR}/third_party/portAudio/include
)

add_dependencies(${libName} portaudio)
target_link_libraries(${libName} PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/portAudio/portaudio_static_x64.lib
)

setupModule(${libName})