add_library(networking OBJECT
  states/offline.cpp

  states/online/netctl.cpp
  states/online/resolver.cpp
  states/online/socket.cpp
  states/online/epoll.cpp
  states/online/http.cpp

  networking.cpp
)

add_dependencies(networking third_party psOff_utility config_emu)

target_compile_definitions(networking PUBLIC _WINSOCK_DEPRECATED_NO_WARNINGS)

file(COPY networking.h DESTINATION ${DST_INCLUDE_DIR}/networking)