
set(BM_SOURCE_FILES
    bm_main.cpp
)

if(WIN32)
    set(BENCHMARK_INC_DIR CACHE STRING "Include directory for google benchmark")
    set(BENCHMARK_LIB_DIR CACHE STRING "Library directory for google benchmark") 
    include_directories(${BENCHMARK_INC_DIR})
    link_directories(${BENCHMARK_LIB_DIR})
else()
    message(STATUS "Benchmark include directory: ${benchmark_INCLUDE_DIR}")
    message(STATUS "Benchmark library directory: ${benchmark_LIBRARIES}")
    include_directories("/usr/local/include")
    link_directories("/usr/local/lib")
endif(WIN32)

add_executable(cd_benchmarks ${BM_SOURCE_FILES})

if (WIN32)
    target_link_libraries(cd_benchmarks benchmark ShLwApi)
else()
    target_link_libraries(cd_benchmarks benchmark)
    target_include_directories(cd_benchmarks PUBLIC)
endif()
