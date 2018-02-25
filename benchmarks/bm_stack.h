#pragma once

#include "../src/cds/stack/locked_stack.h"
#include "../src/cds/stack/lockfree_stack.h"

#include <benchmark/benchmark.h>

#include <random>
#include <memory>

template<typename Stack>
class StackFixture : public benchmark::Fixture 
{
protected:
    StackFixture()
        : m_pStack{ std::make_shared<Stack>() } { }

    virtual void SetUp(benchmark::State& state)
    {
        if (!state.thread_index)
        {
            m_pStack = std::make_shared<Stack>();
            fill(state.max_iterations, 0, 2);
        }
    }

    void fill(const int& n, const int& min, const int& max)
    {
        auto engine = std::default_random_engine{};
        auto dist = std::uniform_int_distribution<int>{ min, max };

        for (auto i = 0; i < n; ++i)
            m_pStack->push(dist(engine));
    }

protected:
    std::shared_ptr<stack::StackBase<int>> m_pStack;
};

//------------------------------------------------------------------------
// Lock-based benchmarks
//------------------------------------------------------------------------

BENCHMARK_TEMPLATE_DEFINE_F(StackFixture, Push, stack::LockedStack<int>)(benchmark::State& state)
{
    for (auto _ : state)
        m_pStack->push(7);

    state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE_DEFINE_F(StackFixture, Pop, stack::LockedStack<int>)(benchmark::State& state)
{
    auto pops = 0;
    auto out = 0;

    for (auto _ : state)
        pops += m_pStack->pop(out) ? 1 : 0;

    state.SetItemsProcessed(pops);
}

//------------------------------------------------------------------------
// Lockfree benchmarks
//------------------------------------------------------------------------

BENCHMARK_TEMPLATE_DEFINE_F(StackFixture, PushLF, stack::LockFreeStack<int>)(benchmark::State& state)
{
    for (auto _ : state)
        m_pStack->push(7);

    state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE_DEFINE_F(StackFixture, PopLF, stack::LockFreeStack<int>)(benchmark::State& state)
{
    auto pops = 0;
    auto out = 0;

    for (auto _ : state)
        pops += m_pStack->pop(out) ? 1 : 0;

    state.SetItemsProcessed(pops);
}

BENCHMARK_REGISTER_F(StackFixture, Push)->DenseThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(StackFixture, Pop)->DenseThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(StackFixture, PushLF)->DenseThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(StackFixture, PopLF)->DenseThreadRange(1, 4)->UseRealTime();
