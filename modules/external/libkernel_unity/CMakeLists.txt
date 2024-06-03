cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libkernel_unity)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} core third_party)

target_link_libraries(${libName} PRIVATE
  core.lib
  libboost_thread
)

target_compile_definitions(${libName} PRIVATE
  BOOST_ALL_NO_LIB
)
setupModule(${libName})