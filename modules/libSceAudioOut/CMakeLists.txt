cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libSceAudioOut)
project(${libName})

add_library(${libName} SHARED entry.cpp)

add_dependencies(${libName} third_party config)

target_link_libraries(${libName} PRIVATE config.lib SDL2 libboost_thread)
target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)
target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/SDL2/include
)

setupModule(${libName})
