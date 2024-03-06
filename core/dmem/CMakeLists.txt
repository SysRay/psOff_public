add_library(dmem OBJECT
  dmem.cpp
)

add_dependencies(dmem third_party psOff_utility)

target_include_directories(dmem PRIVATE
  ${PRJ_SRC_DIR}/third_party/magic_enum/include
)
