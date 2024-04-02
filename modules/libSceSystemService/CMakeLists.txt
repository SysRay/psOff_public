cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceSystemService)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} core config_emu)
target_link_libraries(${libName} PRIVATE core.lib SDL2 ${Vulkan_LIBRARIES} config_emu.lib)
target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)

setupModule(${libName})
