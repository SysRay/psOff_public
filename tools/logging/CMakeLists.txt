cmake_minimum_required(VERSION 3.24)

add_library(logging SHARED logging.cpp)

target_link_directories(logging PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

add_dependencies(logging third_party)

target_link_libraries(logging ws2_32 p7)
target_include_directories(logging PRIVATE ${PRJ_SRC_DIR}/third_party/p7/Headers)
target_compile_options(logging PRIVATE "/Zi")

ADD_CUSTOM_COMMAND(TARGET logging
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/logging.lib" "${CMAKE_BINARY_DIR}/third_party/install/lib/"
)

set_target_properties(logging
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
)