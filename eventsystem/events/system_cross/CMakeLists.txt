add_library(events_system_cross OBJECT
  events.cpp
  handler.cpp
)

target_compile_definitions(events_system_cross PUBLIC _CRT_SECURE_NO_WARNINGS)

add_library(events_intern_system_cross OBJECT
  intern/core.cpp
)

add_dependencies(events_intern_system_cross third_party)

target_compile_definitions(events_intern_system_cross PUBLIC _CRT_SECURE_NO_WARNINGS)
