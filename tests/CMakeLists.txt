enable_testing()

link_libraries(gtest_main gmock gmock_main)
add_link_options(/DEBUG)
link_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/lib
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

include_directories(
  ${PRJ_SRC_DIR}
  ${CMAKE_BINARY_DIR}/third_party/install/include

  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/public_repo
  ${PRJ_SRC_DIR}/public_repo/third_party
  ${PRJ_SRC_DIR}/public_repo/tools/logging
)

add_subdirectory(core)