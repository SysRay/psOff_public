add_library(oscctl OBJECT
  oscctl.cpp
)

file(COPY oscctl.h DESTINATION ${DST_INCLUDE_DIR}/oscctl)
