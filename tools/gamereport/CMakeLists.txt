project(gamereport VERSION 0.0.1)

set(SRC
  entry.cpp
)

add_library(gamereport SHARED ${SRC})

target_include_directories(gamereport PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/third_party/install/include
  ${PRJ_SRC_DIR}/third_party/SDL2/include
)

target_link_directories(gamereport PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

add_dependencies(gamereport third_party)
target_link_libraries(gamereport SDL2 logging.lib)

set_target_properties(gamereport
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/third_party/intall/lib"
)

install(TARGETS gamereport LIBRARY DESTINATION .)
