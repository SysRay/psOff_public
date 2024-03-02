cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAudioOut)
project(${libName})

add_library(${libName} SHARED entry.cpp)
target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/portAudio/include
)

add_dependencies(${libName} third_party)

target_link_libraries(${libName} PUBLIC
  portaudio_static_x64
)

setupModule(${libName})