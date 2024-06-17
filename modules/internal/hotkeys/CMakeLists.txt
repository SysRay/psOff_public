add_library(hotkeys OBJECT
  hotkeys.cpp
)

add_dependencies(hotkeys third_party config_emu)

file(COPY hotkeys.h DESTINATION ${DST_INCLUDE_DIR}/hotkeys)