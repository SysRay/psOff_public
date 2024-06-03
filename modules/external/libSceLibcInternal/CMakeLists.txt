cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceLibcInternal)
project(${libName})

add_library(${libName} SHARED entry.cpp mspace.cpp)
target_compile_definitions(${libName} PRIVATE _CRT_SECURE_NO_WARNINGS)

add_dependencies(${libName} core third_party)
target_link_libraries(${libName} PRIVATE core.lib libboost_thread)

setupModule(${libName})
