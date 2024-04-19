cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceHttp)
project(${libName})

add_library(${libName} SHARED entry.cpp)
add_dependencies(${libName} third_party)
target_link_libraries(${libName} PUBLIC libboost_url)
target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB)

setupModule(${libName})