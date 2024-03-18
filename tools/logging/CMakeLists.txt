cmake_minimum_required(VERSION 3.24)

add_library(logging SHARED logging.cpp)

target_link_directories(logging PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

add_dependencies(logging third_party)

target_link_libraries(logging ws2_32 p7 config.lib libboost_thread)
target_compile_definitions(logging PRIVATE BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)
target_include_directories(logging PRIVATE ${PRJ_SRC_DIR}/third_party/p7/Headers)
target_compile_options(logging PRIVATE "/Zi")

ADD_CUSTOM_COMMAND(TARGET logging
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/logging.lib" "${CMAKE_BINARY_DIR}/lib"
)

set_target_properties(logging
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
)

install(TARGETS logging LIBRARY DESTINATION .)
