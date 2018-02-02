set(CDS_SOURCE_FILES
    stack.h
    locked_stack.h
    locked_stack.cpp
)

add_library(cds STATIC ${CDS_SOURCE_FILES})
