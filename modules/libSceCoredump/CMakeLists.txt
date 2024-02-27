cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceCoredump)
project(${libName})

add_library(${libName} SHARED
  entry.cpp
)


target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB)
target_link_libraries(${libName} PRIVATE
  libboost_thread)

setupModule(${libName})