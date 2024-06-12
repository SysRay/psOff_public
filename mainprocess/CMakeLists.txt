project(psoff VERSION 0.0.1)

set(SRC
  main.cpp
)

add_executable(psoff ${SRC})

target_include_directories(psoff PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/include
  ${CMAKE_BINARY_DIR}/eventsystem/lib
)

add_dependencies(psoff core third_party eventsystem)

target_link_libraries(psoff
  core.lib
  eventsystem
  libboost_program_options
)

target_link_options(psoff PUBLIC /DEBUG)
target_compile_definitions(psoff PRIVATE BOOST_ALL_NO_LIB _CRT_SECURE_NO_WARNINGS)

install(TARGETS psoff
  COMPONENT psoff
  RUNTIME_DEPENDENCIES
  PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
  POST_EXCLUDE_REGEXES ".*system32/.*\\.dll" ".*vulkan-1\\.dll"

  DIRECTORIES
  $<TARGET_FILE_DIR:psoff>
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/bin
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/lib
  ${CMAKE_BINARY_DIR}/eventsystem/lib
  ${CMAKE_BINARY_DIR}/bin
  ${CMAKE_BINARY_DIR}/lib
  ${CMAKE_CURRENT_BINARY_DIR}/core
  ${CMAKE_BINARY_DIR}/psoff_render/src/psoff_render/lib
  ${CMAKE_BINARY_DIR}/core/export/lib
  DESTINATION .
)

install(FILES $<TARGET_PDB_FILE:psoff> DESTINATION ${CMAKE_INSTALL_PREFIX}/debug OPTIONAL)