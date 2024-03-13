cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceCoredump)
project(${libName})

add_library(${libName} SHARED
  entry.cpp
)

add_dependencies(${libName} third_party boost)

target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB)
target_link_libraries(${libName} PRIVATE libboost_thread psOff_utility)

setupModule(${libName})