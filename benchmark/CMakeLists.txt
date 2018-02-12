
set(BM_SOURCE_FILES
    bm_stack.h
    bm_main.cpp
)

add_executable(cd_benchmarks ${BM_SOURCE_FILES})

if (WIN32)
    target_link_libraries(cd_benchmarks benchmark ShLwApi)
else()
    target_link_libraries(cd_benchmarks benchmark)
    target_include_directories(cd_benchmarks /urs/local/include)
endif()
