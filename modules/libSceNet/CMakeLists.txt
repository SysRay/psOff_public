cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceNet)
project(${libName})

add_library(${libName} SHARED entry.cpp epoll.cpp socket.cpp)

setupModule(${libName})