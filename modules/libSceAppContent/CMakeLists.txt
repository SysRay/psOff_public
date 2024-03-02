cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAppContent)
project(${libName})

add_library(${libName} SHARED entry.cpp)

target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/runtime/system
)

add_dependencies(${libName} core)
target_link_libraries(${libName} PRIVATE core.lib)

setupModule(${libName})