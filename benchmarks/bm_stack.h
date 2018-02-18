#pragma once

#include <benchmark/benchmark.h>

class StackFixture : public benchmark::Fixture {};

BENCHMARK_F(StackFixture, PushTest)(benchmark::State& st)
{
    int i = 0;
    for (auto _ : st) {
        i++;
    }
}

BENCHMARK_F(StackFixture, PopTest)(benchmark::State& st)
{
    int i = 0;
    for (auto _ : st) {
        i--;
    }
}

BENCHMARK_REGISTER_F(StackFixture, PushTest)->Threads(4);
BENCHMARK_REGISTER_F(StackFixture, PopTest)->Threads(4);