cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceSaveData)
project(${libName})

add_library(${libName} SHARED entry.cpp)

target_link_libraries(${libName} PRIVATE emulator.lib)

setupModule(${libName})