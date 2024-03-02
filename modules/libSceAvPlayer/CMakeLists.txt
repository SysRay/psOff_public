cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAvPlayer)
project(${libName})
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_WINDOWS_SYMBOL_VISIBILITY_IN_STATIC_LIBRARIES OFF)

add_library(${libName} SHARED
  entry.cpp entryEx.cpp avplayer.cpp
)

target_link_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/ffmpeg/lib
)

target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/ffmpeg/include
  ${Vulkan_INCLUDE_DIRS}
)

set(FFMPEG_LIBS
  avformat
  avcodec
  avutil
  swscale
  swresample
)

add_dependencies(${libName} core)
target_link_libraries(${libName} PUBLIC core.lib psOff_utility ${FFMPEG_LIBS})

setupModule(${libName})