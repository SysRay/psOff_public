set(libName libSceNpGameIntent)
include(../setupModule.cmake)

project(${libName})

add_library(${libName} SHARED entry.cpp)

setupModule(${libName})
