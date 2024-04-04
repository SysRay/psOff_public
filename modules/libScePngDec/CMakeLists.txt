cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libScePngDec)
project(${libName})

add_library(${libName} SHARED entry.cpp)

target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/libpng/
)

add_dependencies(${libName} third_party)
target_link_libraries(${libName} libpng16)

setupModule(${libName})
