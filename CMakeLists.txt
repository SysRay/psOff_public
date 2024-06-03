cmake_minimum_required(VERSION 3.24)
include(ExternalProject)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
set(CMAKE_WINDOWS_SYMBOL_VISIBILITY_IN_STATIC_LIBRARIES OFF)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(PSOFF_LIB_VERSION v.0.3)
set(PSOFF_RENDER_VERSION v.0.6-nightly_03.06.2024)

set(ProjectName psOff_${CMAKE_BUILD_TYPE})
project(${ProjectName} VERSION 0.0.1)

unset(CMAKE_IMPORT_LIBRARY_SUFFIX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INSTALL_MESSAGE LAZY)

if(ISDEBUG)
  set(CMAKE_CXX_FLAGS_RELEASE "/MD /Zi /GS- /GF /Gy /GR /Oi -O0 -fno-strict-aliasing")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:REF,ICF")
else()
  set(CMAKE_CXX_FLAGS_RELEASE "/MD /GS- /GF /Gy /GR /Oi -Ofast -fno-strict-aliasing")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:REF,ICF")
endif()

if(NOT PRJ_SRC_DIR)
  set(PRJ_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set(IMAGE_BASE 0x10000000)

# # Gather Infos

# Vulkan
find_package(Vulkan 1.3 REQUIRED)
message("Vulkan Libs: ${Vulkan_LIBRARIES}")
message("Vulkan Include: ${Vulkan_INCLUDE_DIRS}")

get_filename_component(VulkanPath ${Vulkan_LIBRARY} DIRECTORY)
message("Vulkan Path: ${VulkanPath}")

# # - Gather Infos
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Move all libs here post-build, except third_party
add_definitions(-DPSOFF_RENDER_VERSION="${PSOFF_RENDER_VERSION}")

#
include_directories(BEFORE
  ${CMAKE_INSTALL_PREFIX}/development/include
  ${PRJ_SRC_DIR}/modules_include
  ${PRJ_SRC_DIR}/tools/logging
  ${PRJ_SRC_DIR}/tools/config_emu
  ${PRJ_SRC_DIR}/tools/gamereport
  ${PRJ_SRC_DIR}
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/include
  ${CMAKE_BINARY_DIR}/psoff_render/src/psoff_render/include
)

link_directories(BEFORE
  ${CMAKE_INSTALL_PREFIX}/development/lib
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/lib
  ${CMAKE_BINARY_DIR}/lib
  ${CMAKE_BINARY_DIR}/bin
  ${CMAKE_BINARY_DIR}/psoff_render/src/psoff_render/lib
  ${CMAKE_BINARY_DIR}/core/export/lib
)

ExternalProject_Add(third_party
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  PREFIX ${CMAKE_BINARY_DIR}/third_party
  BUILD_IN_SOURCE 1
  URL https://github.com/SysRay/psOff_thirdParty/releases/download/${PSOFF_LIB_VERSION}/psOff-3rd.zip
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  ExternalProject_Add(psoff_render
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    PREFIX ${CMAKE_BINARY_DIR}/psoff_render
    BUILD_IN_SOURCE 1
    URL https://github.com/SysRay/psOff_Renderer/releases/download/${PSOFF_RENDER_VERSION}/psOff-renderer.zip
  )
endif()

set(TEST_BENCH OFF CACHE BOOL "Enable testing")

if(TEST_BENCH)
 add_subdirectory(tests)
endif()

# include before link_libraries
add_subdirectory(tools/logging)
add_subdirectory(tools/config_emu)
add_subdirectory(tools/dll2Nids)
add_subdirectory(asm)

add_dependencies(dll2Nids third_party)

# -
link_libraries(
  logging.lib
)
add_compile_definitions(IMAGE_BASE=${IMAGE_BASE})

# Internal Projects
add_subdirectory(modules)
add_subdirectory(utility)
add_subdirectory(tools/gamereport)
add_subdirectory(core)

# #- Projects

# Executable
add_executable(psoff
  main.cpp
)

add_dependencies(psoff logging core psOff_utility gamereport)

target_link_libraries(psoff PRIVATE
  core.lib
  gamereport.lib
  psOff_utility
  libboost_filesystem
  $<TARGET_OBJECTS:asmHelper>
)

target_link_options(psoff PUBLIC /DYNAMICBASE:NO --image-base=${IMAGE_BASE})

set_property(TARGET psoff PROPERTY VS_DPI_AWARE "PerMonitor") # SDL2 High dpi window

# # Install
install(DIRECTORY "${CMAKE_BINARY_DIR}/third_party/src/third_party/bin/" DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.dll"
)

install(DIRECTORY "${CMAKE_BINARY_DIR}/bin/" DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.dll"
)

install(DIRECTORY "${CMAKE_BINARY_DIR}/lib/" DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.dll"
)

install(DIRECTORY "${PRJ_SRC_DIR}/docs/json/" DESTINATION ${CMAKE_INSTALL_PREFIX}/config/.schemas
  FILES_MATCHING PATTERN "*.json"
)

if(DEFINED ISACTION)
  file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/bin/vulkan-1.dll CONTENT "") # cmake needs to find the file in order to ignore it
endif()

install(TARGETS psoff
  COMPONENT psoff
  RUNTIME_DEPENDENCIES
  PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
  POST_EXCLUDE_REGEXES ".*system32/.*\\.dll" ".*vulkan-1\\.dll"

  DIRECTORIES
  $<TARGET_FILE_DIR:psoff>
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/bin
  ${CMAKE_BINARY_DIR}/third_party/src/third_party/lib
  ${CMAKE_BINARY_DIR}/bin
  ${CMAKE_BINARY_DIR}/lib
  ${CMAKE_CURRENT_BINARY_DIR}/core
  ${CMAKE_BINARY_DIR}/psoff_render/src/psoff_render/lib
  ${CMAKE_BINARY_DIR}/core/export/lib
  DESTINATION .
)

install(FILES $<TARGET_PDB_FILE:psoff> DESTINATION ${CMAKE_INSTALL_PREFIX}/debug OPTIONAL)
