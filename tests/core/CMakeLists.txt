enable_testing()

add_executable(core_test
  semaphore/entry.cpp
)

add_test(NAME core_test COMMAND core_test)

add_dependencies(core_test third_party psOff_utility logging_stub)

set_target_properties(core_test
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
  PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
)