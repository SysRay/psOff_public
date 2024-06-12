add_subdirectory(events/template)
add_subdirectory(events/system)
add_subdirectory(test)

add_library(eventsystem_core SHARED
  $<TARGET_OBJECTS:events_intern_template>
  $<TARGET_OBJECTS:events_intern_system>
)

add_library(eventsystem STATIC
  $<TARGET_OBJECTS:events_template>
  $<TARGET_OBJECTS:events_system>
)

target_link_libraries(eventsystem_core PUBLIC libboost_serialization)
target_link_directories(eventsystem_core PUBLIC
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/include
)

target_link_libraries(eventsystem PUBLIC eventsystem_core.lib libboost_serialization)
target_link_directories(eventsystem PUBLIC
  ${CMAKE_BINARY_DIR}/eventsystem/lib
)

add_dependencies(eventsystem eventsystem_core)

install(TARGETS eventsystem_core LIBRARY DESTINATION .)

set_target_properties(eventsystem
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/eventsystem/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/eventsystem/lib"
  PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/"
)

set_target_properties(eventsystem_core
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/eventsystem/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/eventsystem/bin"
  PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/"
)
