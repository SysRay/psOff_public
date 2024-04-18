cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceNetCtl)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} config_emu)

target_compile_definitions(${libName} PUBLIC WIN32_LEAN_AND_MEAN _WINSOCK_DEPRECATED_NO_WARNINGS BOOST_ALL_NO_LIB)
target_link_libraries(${libName} PUBLIC IPHLPAPI.lib Ws2_32.lib libboost_thread config_emu.lib ntdll.dll)

setupModule(${libName})
