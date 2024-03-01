cmake_minimum_required(VERSION 3.24)

set(ProjectName psOff_${CMAKE_BUILD_TYPE})
project(${ProjectName} VERSION 0.0.1)

unset(CMAKE_IMPORT_LIBRARY_SUFFIX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "/MD /EHa /Zi /GS- /GF /Gy /GR /Oi -Ofast -fno-strict-aliasing")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:REF,ICF")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_WINDOWS_SYMBOL_VISIBILITY_IN_STATIC_LIBRARIES OFF)

if(NOT PRJ_SRC_DIR)
  set(PRJ_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# # Gather Infos

# Vulkan
find_package(Vulkan 1.3 REQUIRED)
message("Vulkan Libs: ${Vulkan_LIBRARIES}")
message("Vulkan Include: ${Vulkan_INCLUDE_DIRS}")

get_filename_component(VulkanPath ${Vulkan_LIBRARY} DIRECTORY)
message("Vulkan Path: ${VulkanPath}")

# # - Gather Infos

#
include_directories(BEFORE
  ${CMAKE_INSTALL_PREFIX}/development/include
  ${PRJ_SRC_DIR}/modules_include
  ${PRJ_SRC_DIR}
  ${CMAKE_BINARY_DIR}/third_party/install/include
)

link_directories(BEFORE
  ${CMAKE_INSTALL_PREFIX}/development/lib
  ${CMAKE_BINARY_DIR}/third_party/install/lib
  ${CMAKE_BINARY_DIR}/core
)

link_libraries(
  psOff_utility
  logging
)

add_definitions("-D__APICALL_IMPORT")

# # Projects
include("third_party/third_party.cmake")

# Internal Projects
add_subdirectory(modules)
add_subdirectory(core)

# #- Projects

# # Install
install(DIRECTORY "${CMAKE_BINARY_DIR}/third_party/bin/" DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.dll"
)