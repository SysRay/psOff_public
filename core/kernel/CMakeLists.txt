add_library(kernel OBJECT
  eventqueue.cpp
  eventflag.cpp
)

add_dependencies(kernel third_party)