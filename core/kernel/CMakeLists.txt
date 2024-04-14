add_library(kernel OBJECT
  eventqueue.cpp
  eventflag.cpp
  errors.cpp
  filesystem.cpp
  pthread.cpp
  semaphore_fifo.cpp
)

add_dependencies(kernel third_party boost)