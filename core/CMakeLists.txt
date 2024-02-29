# Place before add_subdir -> define for all childs
add_compile_definitions(
  BOOST_ALL_NO_LIB
)

add_subdirectory(kernel)

# add_subdirectory(videoout)
add_library(core SHARED
  $<TARGET_OBJECTS:kernel>

  # $<TARGET_OBJECTS:videoout>
)

target_link_libraries(core PRIVATE
  libboost_thread
  libboost_chrono
)