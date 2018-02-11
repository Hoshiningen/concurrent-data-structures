set(STACK_SRC
    locked_stack.cpp
    lockfree_stack.cpp
)

add_library(stack STATIC ${STACK_SRC})