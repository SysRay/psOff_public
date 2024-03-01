MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")

  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child} AND NOT child STREQUAL "template")
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()

  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions("-D__APICALL_IMPORT")

FOREACH(subdir ${SUBDIRS})
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

install(TARGETS ${SUBDIRS}
  LIBRARY DESTINATION modules)