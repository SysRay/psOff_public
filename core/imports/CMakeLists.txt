add_library(imports OBJECT
  imports.cpp
  exit.cpp
)

add_dependencies(imports third_party psOff_utility initParams)
target_include_directories(imports PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/third_party/optick/src
  ${PRJ_SRC_DIR}
)