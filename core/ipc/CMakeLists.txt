add_library(ipc OBJECT
  ipc.cpp
)

target_compile_definitions(ipc PRIVATE BOOST_ALL_NO_LIB _CRT_SECURE_NO_WARNINGS)
