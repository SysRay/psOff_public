enable_testing()

add_executable(semaphore_test
  entry.cpp

  ${PRJ_SRC_DIR}/core/kernel/semaphore_fifo.cpp
)

add_test(NAME semaphore_test COMMAND semaphore_test)

add_dependencies(semaphore_test third_party psOff_utility logging_stub boost)

target_link_libraries(semaphore_test PRIVATE
  logging_stub.lib
  libboost_thread
  libboost_chrono
)

set_target_properties(semaphore_test
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
  PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
)