cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceLibcInternal)
project(${libName})

add_library(${libName} SHARED entry.cpp mspace.cpp)
target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB)

add_dependencies(${libName} core third_party boost)
target_link_libraries(${libName} PRIVATE core.lib libboost_thread)

setupModule(${libName})
