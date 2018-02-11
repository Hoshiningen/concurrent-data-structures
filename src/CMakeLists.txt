add_subdirectory(cds/stack)
add_subdirectory(utility)

add_executable(cds main.cpp)
target_link_libraries(cds stack utility)