cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAppContent)
project(${libName})

add_library(${libName} SHARED entry.cpp)

target_include_directories(${libName} PRIVATE
  ${CMAKE_SOURCE_DIR}/runtime/system
)

target_link_libraries(${libName} PRIVATE emulator.lib)

setupModule(${libName})