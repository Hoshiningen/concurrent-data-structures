#pragma once

#include "../src/cds/queue/locked_queue.h"
#include "../src/cds/queue/lockfree_queue.h"

#include "../application/command.h"

#include <benchmark/benchmark.h>

#include <random>
#include <memory>
#include <iostream>

//------------------------------------------------------------------------
// Lock-based benchmarks
//------------------------------------------------------------------------

template<typename Queue>
class QueueFixture : public benchmark::Fixture
{
protected:
    using RandomCMD = application::pc::RandomComputationCommand;
    using RandomCMDQueue = queue::QueueBase<RandomCMD>;

protected:
    virtual void SetUp(benchmark::State& state)
    {
        if (!state.thread_index)
        {
            m_pQueue = std::make_shared<Queue>();
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

    std::shared_ptr<RandomCMDQueue> m_pQueue = { nullptr };
};

//------------------------------------------------------------------------
// Lock-based benchmarks
//------------------------------------------------------------------------

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, ProduceConsumeLocked, queue::LockedQueue<application::pc::RandomComputationCommand>)(benchmark::State& state)
{
    RandomCMD out{};
    for (auto _ : state)
    {
        // sequential environment
        if (!state.thread_index && state.threads == 1)
        {
            for (auto i = 0; i < state.max_iterations; ++i)
            { 
                m_pQueue->enqueue({});
                ++produced;
            }

            for (auto i = 0; i < state.max_iterations; ++i)
            {
                m_pQueue->dequeue(out);
                out.execute(std::chrono::nanoseconds(10));
                ++consumed;
            }
        }
        else if (state.thread_index % 2)
        {
            m_pQueue->enqueue({});
            ++produced;
        }
        else
        {         
            if (m_pQueue->dequeue(out));
            {
                out.execute(std::chrono::nanoseconds(10));
                ++consumed;
            }
        }
    }
    
    state.SetItemsProcessed(consumed.load());
}

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, ProduceConsumeLockFree, queue::LockFreeQueue<application::pc::RandomComputationCommand>)(benchmark::State& state)
{
    RandomCMD out{};
    for (auto _ : state)
    {
        // sequential environment
        if (!state.thread_index && state.threads == 1)
        {
            for (auto i = 0; i < state.max_iterations; ++i)
            {
                m_pQueue->enqueue({});
                ++produced;
            }

            for (auto i = 0; i < state.max_iterations; ++i)
            {
                m_pQueue->dequeue(out);
                out.execute(std::chrono::nanoseconds(100));
                ++consumed;
            }
        }
        else if (state.thread_index % 2)
        {
            m_pQueue->enqueue({});
            ++produced;
        }
        else
        {
            if (m_pQueue->dequeue(out));
            {
                out.execute(std::chrono::nanoseconds(100));
                ++consumed;
            }
        }
    }

    state.SetItemsProcessed(consumed.load());
}

//BENCHMARK_REGISTER_F(QueueFixture, ProduceConsumeLocked)->DenseThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, ProduceConsumeLockFree)->DenseThreadRange(1, 10)->UseRealTime();