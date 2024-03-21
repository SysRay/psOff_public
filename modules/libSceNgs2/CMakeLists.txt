cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceNgs2)
project(${libName})

add_library(${libName} SHARED entry.cpp)

target_link_libraries(${libName} PRIVATE core.lib)

setupModule(${libName})
