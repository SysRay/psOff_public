cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceNet)
project(${libName})

add_library(${libName} SHARED entry.cpp wepoll.c epoll.cpp socket.cpp resolver.cpp)

target_link_libraries(${libName} ws2_32)

setupModule(${libName})
