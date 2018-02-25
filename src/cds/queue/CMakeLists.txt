add_library(queue INTERFACE)

target_include_directories(queue INTERFACE 
    ${PROJECT_SOURCE_DIR}/src/cds/queue)