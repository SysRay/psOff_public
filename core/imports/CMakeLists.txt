add_library(imports OBJECT
  imports.cpp
  intercepts.cpp
)

add_dependencies(imports third_party psOff_utility initParams)
target_include_directories(imports PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}
)