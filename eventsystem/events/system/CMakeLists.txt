add_library(events_system OBJECT
  events.cpp
  handler.cpp
)

add_library(events_intern_system OBJECT
  intern/core.cpp
)

add_dependencies(events_intern_system third_party)
