set(libName libSceZlib)
include(../setupModule.cmake)

project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} third_party)

target_link_libraries(${libName} PRIVATE zlib.lib)

setupModule(${libName})
