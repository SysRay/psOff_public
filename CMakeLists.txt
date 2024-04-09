cmake_minimum_required(VERSION 3.24)

set(ProjectName psOff_${CMAKE_BUILD_TYPE})
project(${ProjectName} VERSION 0.0.1)

unset(CMAKE_IMPORT_LIBRARY_SUFFIX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "/MD /Zi /GS- /GF /Gy /GR /Oi -Ofast -fno-strict-aliasing")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:REF,ICF")
set(CMAKE_INSTALL_MESSAGE LAZY)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_WINDOWS_SYMBOL_VISIBILITY_IN_STATIC_LIBRARIES OFF)

if(NOT PRJ_SRC_DIR)
  set(PRJ_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT DEFINED IMAGE_BASE)
  set(IMAGE_BASE 0x10000000)
endif()

# # Gather Infos

# Vulkan
find_package(Vulkan 1.3 REQUIRED)
message("Vulkan Libs: ${Vulkan_LIBRARIES}")
message("Vulkan Include: ${Vulkan_INCLUDE_DIRS}")

get_filename_component(VulkanPath ${Vulkan_LIBRARY} DIRECTORY)
message("Vulkan Path: ${VulkanPath}")

# # - Gather Infos
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Move all libs here post-build, except third_party

#
include_directories(BEFORE
  ${CMAKE_INSTALL_PREFIX}/development/include
  ${PRJ_SRC_DIR}/modules_include
  ${PRJ_SRC_DIR}/tools/logging
  ${PRJ_SRC_DIR}/tools/config_emu
  ${PRJ_SRC_DIR}/tools/gamereport
  ${PRJ_SRC_DIR}
  ${CMAKE_BINARY_DIR}/third_party/install/include
)

link_directories(BEFORE
  ${CMAKE_INSTALL_PREFIX}/development/lib
  ${CMAKE_BINARY_DIR}/third_party/install/lib
  ${CMAKE_BINARY_DIR}/core
  ${CMAKE_BINARY_DIR}/lib
)

add_subdirectory(tools/logging) # include before link_libraries
add_subdirectory(tools/config_emu)
add_subdirectory(tools/gamereport)
add_subdirectory(tools/dll2Nids)
add_dependencies(dll2Nids third_party)

set(TEST_BENCH OFF CACHE BOOL "Enable testing")

if(TEST_BENCH)
  add_subdirectory(tests)
endif()

link_libraries(
  logging.lib
)
add_compile_definitions(IMAGE_BASE=${IMAGE_BASE})

# # Projects
include("third_party/third_party.cmake")

# Internal Projects
add_subdirectory(modules)
add_subdirectory(core)
add_subdirectory(utility)

# #- Projects

# # Install
install(DIRECTORY "${CMAKE_BINARY_DIR}/third_party/bin/" DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.dll"
)
install(DIRECTORY "${CMAKE_BINARY_DIR}/third_party/install/bin/" DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.dll"
)
