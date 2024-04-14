cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libScePosix)
project(${libName})

add_library(${libName} SHARED
  entry.cpp
  fs.cpp
  mman.cpp
  pthread.cpp
)

add_dependencies(${libName} core third_party)
target_link_libraries(${libName} PRIVATE
  core.lib
  libboost_thread
  libboost_chrono
)

target_compile_definitions(${libName} PRIVATE
  BOOST_ALL_NO_LIB
)

setupModule(${libName})