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

add_dependencies(${libName} core psoff_render)
target_link_libraries(${libName} PRIVATE core.lib psoff_render.lib)

setupModule(${libName})