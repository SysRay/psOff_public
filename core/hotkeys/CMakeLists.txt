add_library(hotkeys OBJECT
  hotkeys.cpp
)

add_dependencies(hotkeys third_party config_emu)

target_compile_definitions(hotkeys PUBLIC WIN32_LEAN_AND_MEAN)
