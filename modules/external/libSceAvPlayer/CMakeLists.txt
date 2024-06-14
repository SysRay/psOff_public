cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAvPlayer)
project(${libName})

add_library(${libName} SHARED
  entry.cpp entryEx.cpp avplayer.cpp
)

target_include_directories(${libName} PRIVATE
  ${Vulkan_INCLUDE_DIRS}
)

set(FFMPEG_LIBS
  avformat
  avcodec
  avutil
  swscale
  swresample
)

add_dependencies(${libName} core psoff_render)
target_link_libraries(${libName} PUBLIC core.lib psOff_utility ${FFMPEG_LIBS} psoff_render.lib)

setupModule(${libName})