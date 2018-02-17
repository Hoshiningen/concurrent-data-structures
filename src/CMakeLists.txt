add_subdirectory(cds/stack)
add_subdirectory(utility)

add_executable(cds-exe main.cpp)
target_link_libraries(cds-exe stack utility)