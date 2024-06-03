add_library(runtime OBJECT
  util/exceptionHandler.cpp
  util/moduleLoader.cpp
  util/virtualmemory.cpp

  formats/elf64.cpp
  exports/intern.cpp

  runtimeLinker.cpp
)

add_dependencies(runtime third_party psOff_utility)

file(COPY runtimeLinker.h runtimeExport.h program.h procParam.h DESTINATION ${DST_INCLUDE_DIR}/runtime)
file(COPY exports/intern.h exports/macro.h DESTINATION ${DST_INCLUDE_DIR}/runtime/exports)
file(COPY formats/ISymbols.h formats/IFormat.h DESTINATION ${DST_INCLUDE_DIR}/runtime/formats)