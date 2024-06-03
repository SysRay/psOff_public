add_library(initParams OBJECT
  initParams.cpp
)

add_dependencies(initParams third_party)

file(COPY initParams.h DESTINATION ${DST_INCLUDE_DIR}/initParams)