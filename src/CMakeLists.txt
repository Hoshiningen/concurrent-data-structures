add_subdirectory(cds/stack)
add_subdirectory(cds/queue)
add_subdirectory(cds/hashtable)
add_subdirectory(utility)

add_executable(cds-exe main.cpp)
target_link_libraries(cds-exe stack queue utility)