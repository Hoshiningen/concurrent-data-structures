set(QUEUE_SRC
    locked_queue.cpp
    lockfree_queue.cpp
)

add_library(queue STATIC ${QUEUE_SRC})