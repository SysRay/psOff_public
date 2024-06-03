enable_testing()

add_executable(semaphore_test
  entry.cpp

  ${PRJ_SRC_DIR}/core/kernel/semaphore_fifo.cpp
)

add_test(NAME semaphore_test COMMAND semaphore_test)

add_dependencies(semaphore_test third_party psOff_utility logging_stub)

target_link_libraries(semaphore_test PRIVATE
  logging_stub.lib
  libboost_thread
  libboost_chrono
)