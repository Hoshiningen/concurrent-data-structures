add_subdirectory(cds)

add_executable(concurrent-data-structures main.cpp)

if (WIN32)
    target_link_libraries(concurrent-data-structures cds benchmark ShLwApi)
else()
    target_link_libraries(concurrent-data-structures cds benchmark)
endif()
