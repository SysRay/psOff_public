cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

add_compile_definitions(
  BOOST_ALL_NO_LIB
)

set(libName libSceNgs2)
project(${libName})

add_library(${libName} SHARED entry.cpp readFuncs.cpp reader.cpp)

set(FFMPEG_LIBS
  avformat
  avcodec
  avutil
  swscale
  swresample
)

add_dependencies(${libName} core)
target_link_libraries(${libName} PRIVATE core.lib ${FFMPEG_LIBS} libboost_thread libboost_chrono)

setupModule(${libName})
