cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libScePlayGo)
project(${libName})

add_library(${libName} SHARED
  chunkreader.cpp
  entry.cpp
)

add_dependencies(${libName} core)

target_link_libraries(${libName} PRIVATE core.lib)

setupModule(${libName})
