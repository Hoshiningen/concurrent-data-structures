cmake_minimum_required (VERSION 3.9.2)

project(concurrent-data-structures)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

if(WIN32)
else()
    find_package(benchmark REQUIRED)
endif(WIN32)

set(SDF "asdf")

message(STATUS "Benchmark include directory: ${SDF}")

if(benchmark_FOUND)
    message(STATUS "Benchmark include directory: ${BENCHMARK_INCLUDE_DIRS}")
    message(STATUS "Benchmark library directory: ${BENCHMARK_LIBRARIES}")
    include_directories(${BENCHMARK_INCLUDE_DIRS})
    link_directories(${BENCHMARK_LIBRARIES})
else()
    set(BENCHMARK_INC_DIR CACHE STRING "Include directory for google benchmark")
    set(BENCHMARK_LIB_DIR CACHE STRING "Library directory for google benchmark") 
    include_directories(${BENCHMARK_INC_DIR})
    link_directories(${BENCHMARK_LIB_DIR})
endif(benchmark_FOUND)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(benchmark)