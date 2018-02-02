cmake_minimum_required (VERSION 3.9.2)

project(concurrent-data-structures)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(BENCHMARK_DIR CACHE STRING "Base directory for google benchmark")
message(STATUS "Google Benchmark directory is: ${BENCHMARK_DIR}")

include_directories("${BENCHMARK_DIR}/include")
link_directories("${BENCHMARK_DIR}/build/x86-Release/src")

# Add subdirectories
add_subdirectory(src)
add_subdirectory(benchmark)