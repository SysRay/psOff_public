cmake_minimum_required(VERSION 3.24)

set(IMGUI_ROOT ${PRJ_SRC_DIR}/third_party/imgui)

add_library(emui SHARED emui.cpp
  ${IMGUI_ROOT}/imgui.cpp
  ${IMGUI_ROOT}/imgui_draw.cpp
  ${IMGUI_ROOT}/imgui_tables.cpp
  ${IMGUI_ROOT}/imgui_widgets.cpp
  ${IMGUI_ROOT}/imgui_demo.cpp
  ${IMGUI_ROOT}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_ROOT}/backends/imgui_impl_vulkan.cpp
)

target_link_directories(emui PRIVATE
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

target_include_directories(emui PRIVATE
  ${Vulkan_INCLUDE_DIRS}
  ${PRJ_SRC_DIR}/third_party/SDL2/include
  ${IMGUI_ROOT}/backends/
  ${IMGUI_ROOT}/
)

add_dependencies(emui third_party)

target_link_libraries(emui PRIVATE SDL2.lib  ${Vulkan_LIBRARIES})
# target_compile_definitions(emui PRIVATE BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)
target_compile_options(emui PRIVATE "/Zi")

set_target_properties(emui
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/."
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/third_party/intall/lib"
)

install(TARGETS emui LIBRARY DESTINATION .)
