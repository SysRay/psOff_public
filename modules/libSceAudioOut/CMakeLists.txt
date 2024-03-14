cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAudioOut)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} third_party)

target_link_libraries(${libName} PRIVATE SDL2)
target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/SDL2/include
)

setupModule(${libName})
