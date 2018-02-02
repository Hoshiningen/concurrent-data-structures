
set(BM_SOURCE_FILES
)

add_executable(cd_benchmarks ${BM_SOURCE_FILES})

if (WIN32)
    target_link_libraries(cd_benchmarks benchmark ShLwApi)
else()
    target_link_libraries(cd_benchmarks benchmark)
endif()
