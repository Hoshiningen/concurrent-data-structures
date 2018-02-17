cmake_minimum_required (VERSION 3.9.2)

project(concurrent-data-structures)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

find_package(benchmark)

if(benchmark_FOUND)
    message(STATUS "Benchmark include directory: ${benchmark_INCLUDE_DIRS}")
    message(STATUS "Benchmark library directory: ${benchmark_LIBRARIES}")
    include_directories(${benchmark_INCLUDE_DIRS})
    link_directories(${benchmark_LIBRARIES})
else()
    set(BENCHMARK_INC_DIR CACHE STRING "Include directory for google benchmark")
    set(BENCHMARK_LIB_DIR CACHE STRING "Library directory for google benchmark") 
    include_directories(${BENCHMARK_INC_DIR})
    link_directories(${BENCHMARK_LIB_DIR})
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(benchmark)