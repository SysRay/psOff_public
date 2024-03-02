cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceVideoOut)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} core)
target_link_libraries(${libName} PRIVATE core.lib)

setupModule(${libName})