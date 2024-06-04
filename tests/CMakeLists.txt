enable_testing()

link_libraries(gtest_main gmock gmock_main)
add_link_options(/DEBUG)
link_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/lib
)

include_directories(
  ${PRJ_SRC_DIR}

  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/tools/logging
)

add_subdirectory(core)

install(TARGETS semaphore_test RUNTIME DESTINATION .)