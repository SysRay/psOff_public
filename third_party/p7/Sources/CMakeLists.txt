cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(p7)

include_directories(${PATH_SHARED})
include_directories(${PATH_SHARED_PLATFORM})
include_directories(${PATH_P7_API})

if(WIN32)
    set(P7_PLATFORM_LIBS Ws2_32) 
elseif(UNIX)
    set(P7_PLATFORM_LIBS rt pthread)
else()
    message(FATAL_ERROR "${PROJECT_NAME}: Platform not supported")
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} P7_SOURCES)

add_library(${PROJECT_NAME} STATIC ${P7_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${P7_PLATFORM_LIBS})

if(P7_BUILD_SHARED)
    add_library(${PROJECT_NAME}-shared SHARED ${P7_SOURCES})
    target_link_libraries(${PROJECT_NAME}-shared ${P7_PLATFORM_LIBS})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

if(P7_BUILD_SHARED)
    set_property(TARGET ${PROJECT_NAME}-shared PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()


target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Headers/")
if(P7_BUILD_SHARED)
    target_include_directories(${PROJECT_NAME}-shared PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Headers/")
endif()

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../Headers/")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Shared/")

if(WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Shared/Platforms/Windows_x86/")
elseif(UNIX)
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Shared/Platforms/Linux_x86/")
endif()

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        )

if(P7_BUILD_SHARED)
    install(TARGETS ${PROJECT_NAME}-shared
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)
endif()

if(COMMAND set_ide_folder)
    set_ide_folder(${PROJECT_NAME})
    if(P7_BUILD_SHARED)
        set_ide_folder(${PROJECT_NAME}-shared)
    endif()
endif()
