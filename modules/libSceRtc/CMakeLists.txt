cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceRtc)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} third_party boost)
target_link_libraries(${libName} PRIVATE
  libboost_chrono
)

target_compile_definitions(${libName} PRIVATE
  BOOST_ALL_NO_LIB
)

setupModule(${libName})