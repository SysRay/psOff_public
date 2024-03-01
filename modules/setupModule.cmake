function(setupModule _Target)
  add_custom_command(TARGET ${_Target} POST_BUILD
    COMMAND ${CMAKE_INSTALL_PREFIX}/development/bin/dll2Nids.exe ${CMAKE_CURRENT_BINARY_DIR}/${_Target}.dll
  )
  add_dependencies(${_Target} logging)
  install(FILES $<TARGET_PDB_FILE:${_Target}> DESTINATION debug OPTIONAL)
endfunction()