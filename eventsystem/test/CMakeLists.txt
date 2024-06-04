enable_testing()

add_executable(eventsystem_test
  test.cpp
)

target_link_libraries(eventsystem_test PRIVATE
  gtest_main gmock gmock_main eventsystem
)

add_dependencies(eventsystem_test eventsystem)

target_link_directories(eventsystem_test PRIVATE
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(eventsystem_test PRIVATE
  ..

  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/tools/logging
)

install(TARGETS eventsystem_test RUNTIME DESTINATION .)

enable_testing()

add_test(NAME eventsystem_test COMMAND eventsystem_test)
