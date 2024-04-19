add_library(networking OBJECT
  states/offline.cpp

  states/online/netctl.cpp
  states/online/resolver.cpp
  states/online/socket.cpp
  states/online/epoll.cpp
  wepoll.c

  networking.cpp
)

add_dependencies(networking third_party psOff_utility config_emu)

target_compile_definitions(networking PUBLIC WIN32_LEAN_AND_MEAN _WINSOCK_DEPRECATED_NO_WARNINGS)
