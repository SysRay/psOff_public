set(libName libSceUserService)
include(../setupModule.cmake)

project(${libName})

add_library(${libName} SHARED entry.cpp)

setupModule(${libName})