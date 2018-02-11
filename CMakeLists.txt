cmake_minimum_required (VERSION 3.9.2)

project(concurrent-data-structures)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")


if(WIN32)
    set(BENCHMARK_DIR CACHE STRING "Base directory for google benchmark") 
    include_directories("${BENCHMARK_DIR}/include")
    link_directories("${BENCHMARK_DIR}/build/x86-Release/src")
else()
    set(CMAKE_REQUIRED_INCLUDES /usr/local/include)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(benchmark)