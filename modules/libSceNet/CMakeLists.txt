cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceNet)
project(${libName})

add_library(${libName} SHARED entry.cpp epoll.cpp socket.cpp resolver.cpp)

add_dependencies(${libName} third_party core)

target_compile_definitions(${libName} PUBLIC WIN32_LEAN_AND_MEAN)

target_link_libraries(${libName} ws2_32 core.lib)

setupModule(${libName})
