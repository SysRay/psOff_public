add_library(dmem OBJECT
  dmem.cpp
)

add_dependencies(dmem third_party psOff_utility)