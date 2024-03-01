include(ExternalProject)

ExternalProject_Add(third_party
  SOURCE_DIR ${PRJ_SRC_DIR}/third_party
  BINARY_DIR ${CMAKE_BINARY_DIR}/third_party
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=Release
  -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/third_party/install
  -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
  -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
  -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
)