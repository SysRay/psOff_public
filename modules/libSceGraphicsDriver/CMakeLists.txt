cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceGraphicsDriver)
project(${libName})

add_library(${libName} SHARED
  entry.cpp
)

target_include_directories(${libName} PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/tools/sb2spirv/lib/include
)

target_link_libraries(${libName} PRIVATE emulator.lib)

setupModule(${libName})