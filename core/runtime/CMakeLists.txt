add_subdirectory(asm)

add_library(runtime OBJECT
  util/exceptionHandler.cpp
  util/moduleLoader.cpp
  util/virtualmemory.cpp

  formats/elf64.cpp
  exports/intern.cpp

  runtimeLinker.cpp
)

add_dependencies(runtime third_party psOff_utility)