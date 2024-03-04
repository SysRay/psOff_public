cmake_minimum_required(VERSION 3.24)
include(../setupModule.cmake)

set(libName libkernel)
project(${libName})

add_library(${libName} SHARED
  entry.cpp
  equeue.cpp
  eventflag.cpp
  semaphore.cpp
  dmem.cpp
)

add_dependencies(${libName} core third_party)
target_link_libraries(${libName} PRIVATE
  core.lib
  libboost_thread
  libboost_chrono
  bcrypt.lib
)

target_compile_definitions(${libName} PRIVATE
  BOOST_ALL_NO_LIB
)

setupModule(${libName})