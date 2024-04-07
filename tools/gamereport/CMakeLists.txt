project(gamereport VERSION 0.0.1)

set(SRC
  entry.cpp
)

set(GIT_FAIL_IF_NONZERO_EXIT FALSE)
set(GIT_WORKING_DIR ${PRJ_SRC_DIR})
set(PRE_CONFIGURE_FILE "git_ver.cpp.in")
set(POST_CONFIGURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/git_ver.cpp")
include(git_watcher.cmake)

add_library(gamereport SHARED ${SRC} ${POST_CONFIGURE_FILE})

target_include_directories(gamereport PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${PRJ_SRC_DIR}/third_party
  ${CMAKE_BINARY_DIR}/third_party/install/include
  ${PRJ_SRC_DIR}/third_party/SDL2/include
)

target_link_directories(gamereport PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

add_dependencies(gamereport third_party boost check_git)
target_link_libraries(gamereport PUBLIC libboost_url SDL2 logging.lib config_emu.lib)
target_compile_definitions(gamereport PUBLIC BOOST_ALL_NO_LIB)

set_target_properties(gamereport
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/third_party/intall/lib"
)

install(TARGETS gamereport LIBRARY DESTINATION .)
