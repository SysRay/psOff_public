add_library(fileManager OBJECT
  fileManager.cpp
  types/type_in.cpp
  types/type_out.cpp
  types/type_zero.cpp
  types/type_null.cpp
  types/type_file.cpp
  types/type_random.cpp
)

add_dependencies(fileManager third_party psOff_utility)

target_include_directories(fileManager PRIVATE
  ${PRJ_SRC_DIR}/third_party/magic_enum/include
)
