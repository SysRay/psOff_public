cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libScePad)
project(${libName})

add_library(${libName} SHARED entry.cpp
  interfaces/isdl.cpp
  interfaces/ixip.cpp
  interfaces/ikbd.cpp
  cconfig.cpp
)

add_dependencies(${libName} core config_emu)
target_link_libraries(${libName} PRIVATE SDL2 ${Vulkan_LIBRARIES} core.lib config_emu.lib libboost_thread)
target_compile_definitions(${libName} PRIVATE BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)
target_include_directories(${libName} PRIVATE
  ${PRJ_SRC_DIR}/third_party/SDL2/include
)

setupModule(${libName})
