cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceSaveData)
project(${libName})

add_library(${libName} SHARED entry.cpp)

include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/libzip
)

add_dependencies(${libName} core third_party)
target_link_libraries(${libName} PRIVATE core.lib zip.lib zlib.lib)

setupModule(${libName})
