set(BM_SRC
    bm_main.cpp
)

if(WIN32)
    set(BENCHMARK_INC_DIR CACHE STRING "Include directory for google benchmark")
    set(BENCHMARK_LIB_DIR CACHE STRING "Library directory for google benchmark")
    
    list(APPEND LINK_LIBS ShLwApi)
else()
    set(BENCHMARK_INC_DIR "/u/css/smithbd6/Thesis/benchmark/include")
    set(BENCHMARK_LIB_DIR "/u/css/smithbd6/Thesis/build/src")
endif(WIN32)

include_directories(${BENCHMARK_INC_DIR})
include_directories(../src/cds/stack)
include_directories(../src/cds/queue)
include_directories(../application)

link_directories(${BENCHMARK_LIB_DIR})

list(APPEND LINK_LIBS benchmark)

add_executable(cd_benchmarks ${BM_SRC})
target_link_libraries(cd_benchmarks ${LINK_LIBS})
target_include_directories(cd_benchmarks PUBLIC ${BENCHMARK_INC_DIR})