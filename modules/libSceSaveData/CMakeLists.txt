cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceSaveData)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} emuExports core)
target_link_libraries(${libName} PRIVATE emuEemulator.lib xports core)

setupModule(${libName})