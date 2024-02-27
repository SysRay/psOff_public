cmake_minimum_required(VERSION 3.24)

set(ProjectName psOff_${CMAKE_BUILD_TYPE})
project(${ProjectName} VERSION 0.0.1)

unset(CMAKE_IMPORT_LIBRARY_SUFFIX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "/MD /EHa /Zi /GS- /GF /Gy /GR /Oi -Ofast -fno-strict-aliasing")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_WINDOWS_SYMBOL_VISIBILITY_IN_STATIC_LIBRARIES OFF)

find_package(Vulkan 1.3 REQUIRED)
message("Vulkan Libs: ${Vulkan_LIBRARIES}")
message("Vulkan Include: ${Vulkan_INCLUDE_DIRS}")

get_filename_component(VulkanPath ${Vulkan_LIBRARY} DIRECTORY)
message("Vulkan Path: ${VulkanPath}")


if( NOT EMULATOR_DEV_ROOT)
  message(FATAL_ERROR "EMULATOR_DEV_ROOT not defined")
endif()

include_directories(BEFORE
  ${EMULATOR_ROOT}/include
  ${CMAKE_SOURCE_DIR}/modules_include
  ${CMAKE_SOURCE_DIR}
)

link_directories(BEFORE
  ${EMULATOR_ROOT}/lib
)

link_libraries(
  psOff_utility
  logging
)

add_definitions("-D__APICALL_IMPORT")
add_subdirectory(third_party/portAudio)
add_subdirectory(modules)
