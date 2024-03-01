cmake_minimum_required(VERSION 3.24)
project(third_party VERSION 0.0.1)

include(ExternalProject)

message(CHECK_START "-> Building third_party")

add_subdirectory(portAudio)
add_subdirectory(optick)

install(DIRECTORY "ffmpeg/bin/" DESTINATION "${CMAKE_BINARY_DIR}/bin"
  FILES_MATCHING PATTERN "*.dll"
)

set_target_properties(OptickCore
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/lib"
)

# Boost
message(CHECK_START "--> Building boost")
set(BOOST_INCLUDE_LIBRARIES "program_options;date_time;interprocess;stacktrace;uuid;beast;signals2;thread")
ExternalProject_Add(boost_thirdParty
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boost
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/boost
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=Release
  -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/install
  -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
  -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
  -DBoost_USE_STATIC_LIBS=ON
  -DBoost_USE_MULTITHREADED=ON
  -DBUILD_TESTING=OFF
  -DBOOST_INSTALL_LAYOUT=system
  CMAKE_CACHE_ARGS -DBOOST_INCLUDE_LIBRARIES:STRING=${BOOST_INCLUDE_LIBRARIES}
)
message(CHECK_START "<-- boost")


message(CHECK_PASS "<- third_party build")