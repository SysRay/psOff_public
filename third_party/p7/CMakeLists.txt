cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

#project(p7lib)

#>>Build options **********************************************************************
option(P7_TESTS_BUILD "Build test" OFF)
option(P7_EXAMPLES_BUILD "Build examples" OFF)
set(P7_BUILD_SHARED OFF CACHE INTERNAL "Build shared library")
#<<Build options **********************************************************************

include(CheckCXXCompilerFlag)
function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT DEFINED ROOT_P7_PATH)
    set(ROOT_P7_PATH ${PROJECT_SOURCE_DIR} CACHE INTERNAL "")
    set(PATH_P7 ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
    set(PATH_P7_API "${PROJECT_SOURCE_DIR}/Headers" CACHE INTERNAL "")
else(expression)
endif(NOT DEFINED ROOT_P7_PATH)

if(MSVC)
    add_definitions(-D_UNICODE)
    add_definitions(-DUNICODE)

    if(NOT COMMAND set_ide_folder)
        macro(set_ide_folder _project_name)
            string(LENGTH ${ROOT_P7_PATH} ROOT_P7_PATH_LEN)
            string(LENGTH ${CMAKE_CURRENT_SOURCE_DIR} CURRENT_SOURCE_DIR_LEN)
            MATH(EXPR ROOT_P7_PATH_LEN "${ROOT_P7_PATH_LEN}+1")
            MATH(EXPR CURRENT_SOURCE_DIR_LEN "${CURRENT_SOURCE_DIR_LEN} - ${ROOT_P7_PATH_LEN}")
            
            string(SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_P7_PATH_LEN} ${CURRENT_SOURCE_DIR_LEN} IDE_SUBFOLDER)
            set_property(TARGET ${_project_name} PROPERTY FOLDER ${IDE_SUBFOLDER})
        endmacro()
    endif()
else()
    enable_cxx_compiler_flag_if_supported("-Wall")
    enable_cxx_compiler_flag_if_supported("-Wextra")
    enable_cxx_compiler_flag_if_supported("-pedantic")
    enable_cxx_compiler_flag_if_supported("-D_FILE_OFFSET_BITS=64")
    enable_cxx_compiler_flag_if_supported("-D_LARGEFILE64_SOURCE")
    enable_cxx_compiler_flag_if_supported("-fvisibility=hidden")
    enable_cxx_compiler_flag_if_supported("-Wno-variadic-macros")
    enable_cxx_compiler_flag_if_supported("-Wno-long-long")
    enable_cxx_compiler_flag_if_supported("-Wno-missing-field-initializers")
    enable_cxx_compiler_flag_if_supported("-Wno-unknown-pragmas")
    enable_cxx_compiler_flag_if_supported("-fPIC")
    
    macro(set_ide_folder _project_name)
    endmacro()
endif()


add_subdirectory(Headers)
add_subdirectory(Shared)
add_subdirectory(Sources)

if(P7_TESTS_BUILD)
    add_subdirectory(Tests)
endif()

if(P7_EXAMPLES_BUILD)
    add_subdirectory(Examples)
endif()

