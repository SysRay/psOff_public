add_library(timer OBJECT
  timer.cpp
)

add_dependencies(timer third_party)

file(COPY timer.h DESTINATION ${DST_INCLUDE_DIR}/timer)