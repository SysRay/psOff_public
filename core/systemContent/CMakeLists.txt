add_library(systemContent OBJECT
systemContent.cpp
)

add_dependencies(systemContent third_party psOff_utility)

file(COPY systemContent.h DESTINATION ${DST_INCLUDE_DIR}/systemContent)