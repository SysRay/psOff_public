cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAudioOut)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} third_party config_emu core)

target_link_libraries(${libName} PRIVATE core.lib config_emu.lib SDL2 libboost_thread)

target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)

setupModule(${libName})
