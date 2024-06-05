add_subdirectory(events/template)
add_subdirectory(test)
add_subdirectory(ipc)

add_library(eventsystem_core SHARED
  $<TARGET_OBJECTS:events_intern_template>
  $<TARGET_OBJECTS:ipc>
)

add_library(eventsystem STATIC
  $<TARGET_OBJECTS:events_template>
)

target_link_libraries(eventsystem PUBLIC eventsystem_core.lib)
target_link_directories(eventsystem PUBLIC ${CMAKE_BINARY_DIR}/eventsystem/lib)

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
