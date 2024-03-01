
add_library(psOff_utility STATIC utility.cpp)
add_dependencies(psOff_utility logging)
target_link_libraries(psOff_utility PRIVATE logging.lib)