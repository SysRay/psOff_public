cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceNgs2)
project(${libName})

add_library(${libName} SHARED entry.cpp)

target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/ffmpeg/include
)

target_link_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/ffmpeg/lib
)

set(FFMPEG_LIBS
  avformat
  avcodec
  avutil
  swscale
  swresample
)

add_dependencies(${libName} core)
target_link_libraries(${libName} PRIVATE core.lib ${FFMPEG_LIBS})

setupModule(${libName})
