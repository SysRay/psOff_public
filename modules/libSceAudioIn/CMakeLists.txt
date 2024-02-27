cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAudioIn)
project(${libName})

add_library(${libName} SHARED entry.cpp)

target_link_directories(${libName} PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

setupModule(${libName})