function(setupModule _Target)
  add_custom_command(TARGET ${_Target} POST_BUILD
    COMMAND $<TARGET_FILE:dll2Nids> ${CMAKE_CURRENT_BINARY_DIR}/${_Target}.dll
  )
  add_dependencies(${_Target} logging dll2Nids)
  install(FILES $<TARGET_PDB_FILE:${_Target}> DESTINATION debug OPTIONAL)
endfunction()