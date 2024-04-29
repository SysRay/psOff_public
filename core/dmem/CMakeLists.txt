add_library(dmem OBJECT
  dmem.cpp

  memoryManager.cpp
  types/directmem.cpp
)

add_dependencies(dmem third_party psOff_utility)

target_include_directories(dmem PRIVATE
  ${Vulkan_INCLUDE_DIRS}
)