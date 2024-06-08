project(mainipc VERSION 0.0.1)

set(SRC
  main.cpp
  pipeprocess.cpp
)

add_executable(mainipc ${SRC})

target_include_directories(mainipc PRIVATE
  ${CMAKE_SOURCE_DIR}
)

add_dependencies(mainipc core)

target_link_libraries(mainipc core.lib)

target_link_options(mainipc PUBLIC /DEBUG)
