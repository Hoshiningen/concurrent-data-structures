cmake_minimum_required (VERSION 2.8.11)

project(concurrent-data-structures)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -latomic -lpthread -mcx16")
endif(WIN32)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(benchmarks)