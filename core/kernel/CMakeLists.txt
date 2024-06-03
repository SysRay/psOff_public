add_library(kernel OBJECT
  eventqueue.cpp
  eventflag.cpp
  errors.cpp
  filesystem.cpp
  pthread.cpp
  semaphore_fifo.cpp
)

add_dependencies(kernel third_party)

file(COPY errors.h eventflag.h eventqueue_types.h eventqueue.h filesystem.h pthread.h pthread_types.h eventqueue.h semaphore.h DESTINATION ${DST_INCLUDE_DIR}/kernel)