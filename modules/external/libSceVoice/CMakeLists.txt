cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceVoice)
project(${libName})

add_library(${libName} SHARED entry.cpp)

setupModule(${libName})
