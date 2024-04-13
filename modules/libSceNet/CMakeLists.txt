cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceNet)
project(${libName})

add_library(${libName} SHARED entry.cpp wepoll.c epoll.cpp socket.cpp resolver.cpp)

add_dependencies(${libName} boost config_emu)
target_compile_definitions(${libName} PUBLIC BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)
target_link_libraries(${libName} ws2_32 libboost_thread config_emu.lib)

setupModule(${libName})
