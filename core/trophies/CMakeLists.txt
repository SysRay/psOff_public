add_library(trophies OBJECT
  trophies.cpp
)

add_dependencies(trophies third_party)

file(COPY trophies.h DESTINATION ${DST_INCLUDE_DIR}/trophies)