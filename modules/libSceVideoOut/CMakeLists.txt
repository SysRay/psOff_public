set(libName libSceVideoOut)
include(../setupModule.cmake)

project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} core)
target_link_libraries(${libName} PRIVATE core.lib)

setupModule(${libName})
