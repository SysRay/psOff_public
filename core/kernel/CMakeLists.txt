add_library(kernel OBJECT
  eventqueue.cpp
  eventflag.cpp
  errors.cpp
  filesystem.cpp
  pthread.cpp
)

add_dependencies(kernel third_party)