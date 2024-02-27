cmake_minimum_required(VERSION 3.24)
include(ExternalProject)

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

# # Gather Infos
# Emulator development
if(NOT EMULATOR_DEV_ROOT)
  message(FATAL_ERROR "EMULATOR_DEV_ROOT not defined")
endif()

# Vulkan
find_package(Vulkan 1.3 REQUIRED)
message("Vulkan Libs: ${Vulkan_LIBRARIES}")
message("Vulkan Include: ${Vulkan_INCLUDE_DIRS}")

get_filename_component(VulkanPath ${Vulkan_LIBRARY} DIRECTORY)
message("Vulkan Path: ${VulkanPath}")

# # - Gather Infos

#
include_directories(BEFORE
  ${EMULATOR_DEV_ROOT}/include
  ${CMAKE_SOURCE_DIR}/modules_include
  ${CMAKE_SOURCE_DIR}
)

link_directories(BEFORE
  ${EMULATOR_DEV_ROOT}/lib
  ${CMAKE_BINARY_DIR}/third_party/install/lib
)

link_libraries(
  psOff_utility
  logging
)

add_definitions("-D__APICALL_IMPORT")

# # Projects
# Build third party
ExternalProject_Add(third_party
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party
  BINARY_DIR ${CMAKE_BINARY_DIR}/third_party
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=Release
  -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/third_party/install
  -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
  -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
  -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
)

# Internal Projects
add_subdirectory(modules)

# #- Projects

# # Install
install(DIRECTORY "${CMAKE_BINARY_DIR}/third_party/bin/" DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.dll"
)