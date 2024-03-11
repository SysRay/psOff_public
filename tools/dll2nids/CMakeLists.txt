project(dll2Nids VERSION 0.0.1)

set(SRC
  main.cpp
)

add_executable(dll2Nids ${SRC})

target_include_directories(dll2Nids PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/third_party/install/include
)

target_link_directories(dll2Nids PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

add_dependencies(dll2Nids third_party boost)
target_link_libraries(dll2Nids libboost_container)