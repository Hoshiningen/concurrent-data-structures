#pragma once

#include "../src/cds/stack/locked_stack.h"
#include "../src/cds/stack/lockfree_stack.h"

#include "../application/command.h"

#include <benchmark/benchmark.h>

#include <random>
#include <memory>
#include <iostream>

//------------------------------------------------------------------------
// Lock-based benchmarks
//------------------------------------------------------------------------

template<typename Stack>
class StackFixture : public benchmark::Fixture
{
protected:
    using RandomCMD = application::pc::RandomComputationCommand;
    using RandomCMDStack = stack::StackBase<RandomCMD>;

protected:
    virtual void SetUp(benchmark::State& state)
    {
        if (!state.thread_index)
        {
            m_pStack = std::make_shared<Stack>();
        }
    }

    virtual void TearDown(benchmark::State& state)
    {
        if (!state.thread_index)
        {
            produced = 0;
            consumed = 0;
        }
    }

protected:
    std::atomic<int> count = { 0 };
    std::atomic<int> produced = { 0 };
    std::atomic<int> consumed = { 0 };

    std::shared_ptr<RandomCMDStack> m_pStack = { nullptr };
};

//------------------------------------------------------------------------
// Lock-based benchmarks
//------------------------------------------------------------------------

BENCHMARK_TEMPLATE_DEFINE_F(StackFixture, ProduceConsumeLocked, stack::LockedStack<application::pc::RandomComputationCommand>)(benchmark::State& state)
{
    RandomCMD out{};
    for (auto _ : state)
    {
        // sequential environment
        if (!state.thread_index && state.threads == 1)
        {
            for (auto i = 0; i < state.max_iterations; ++i)
            { 
                m_pStack->push({});
                ++produced;
            }

            for (auto i = 0; i < state.max_iterations; ++i)
            {
                m_pStack->pop(out);
                out.execute(std::chrono::nanoseconds(10));
                ++consumed;
            }
        }
        else if (state.thread_index % 2)
        {
            m_pStack->push({});
            ++produced;
        }
        else
        {         
            if (m_pStack->pop(out));
            {
                out.execute(std::chrono::nanoseconds(10));
                ++consumed;
            }
        }
    }
    
    state.SetItemsProcessed(consumed.load());
}

BENCHMARK_TEMPLATE_DEFINE_F(StackFixture, ProduceConsumeLockFree, stack::LockFreeStack<application::pc::RandomComputationCommand>)(benchmark::State& state)
{
    RandomCMD out{};
    for (auto _ : state)
    {
        // sequential environment
        if (!state.thread_index && state.threads == 1)
        {
            for (auto i = 0; i < state.max_iterations; ++i)
            {
                m_pStack->push({});
                ++produced;
            }

            for (auto i = 0; i < state.max_iterations; ++i)
            {
                m_pStack->pop(out);
                out.execute(std::chrono::nanoseconds(10));
                ++consumed;
            }
        }
        else if (state.thread_index % 2)
        {
            m_pStack->push({});
            ++produced;
        }
        else
        {
            if (m_pStack->pop(out));
            {
                out.execute(std::chrono::nanoseconds(10));
                ++consumed;
            }
        }
    }

    state.SetItemsProcessed(consumed.load());
}

//BENCHMARK_REGISTER_F(StackFixture, ProduceConsumeLocked)->DenseThreadRange(1, 4)->UseRealTime();
//BENCHMARK_REGISTER_F(StackFixture, ProduceConsumeLockFree)->DenseThreadRange(1, 4)->UseRealTime();