add_library(ipc OBJECT
  ipc.cpp
)

target_compile_definitions(ipc PUBLIC WIN32_LEAN_AND_MEAN)
