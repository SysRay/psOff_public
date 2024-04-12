set(libName libSceUserService)
include(../setupModule.cmake)

project(${libName})

add_library(${libName} SHARED entry.cpp)

target_link_directories(${libName} PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

add_dependencies(${libName} boost config_emu)
target_compile_definitions(${libName} PUBLIC BOOST_ALL_NO_LIB)
target_link_libraries(${libName} PUBLIC libboost_thread config_emu.lib)

setupModule(${libName})
