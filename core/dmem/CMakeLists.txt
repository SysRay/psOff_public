add_library(dmem OBJECT
  memoryManager.cpp

  types/directmem.cpp
  types/flexiblemem.cpp
)

add_dependencies(dmem third_party psOff_utility)

target_include_directories(dmem PRIVATE
  ${Vulkan_INCLUDE_DIRS}
)

file(COPY memoryManager.h imemory.h DESTINATION ${DST_INCLUDE_DIR}/dmem)