add_library(memory OBJECT
  memory.cpp
)

add_dependencies(memory third_party psOff_utility)

file(COPY memory.h DESTINATION ${DST_INCLUDE_DIR}/memory)