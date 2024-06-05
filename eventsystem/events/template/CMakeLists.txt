add_library(events_template OBJECT
  events.cpp
  handler.cpp
)

add_library(events_intern_template OBJECT
  intern/core.cpp
)

add_dependencies(events_template third_party)
add_dependencies(events_intern_template third_party)