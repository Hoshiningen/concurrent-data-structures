#pragma once

#include "../src/cds/queue/locked_queue.h"
#include "../src/cds/queue/lockfree_queue.h"

#include <benchmark/benchmark.h>

#include <random>
#include <memory>

template<typename Queue>
class QueueFixture : public benchmark::Fixture 
{
protected:
    QueueFixture()
        : m_pQueue{ std::make_shared<Queue>() } { }

    virtual void SetUp(benchmark::State& state)
    {
        if (!state.thread_index)
        {
            m_pQueue = std::make_shared<Queue>();
            fill(state.max_iterations, 0, 2);
        }
    }

    void fill(const int& n, const int& min, const int& max)
    {
        auto engine = std::default_random_engine{};
        auto dist = std::uniform_int_distribution<int>{ min, max };

        for (auto i = 0; i < n; ++i)
            m_pQueue->enqueue(dist(engine));
    }

protected:
    std::shared_ptr<queue::QueueBase<int>> m_pQueue;
};

//------------------------------------------------------------------------
// Lock-based benchmarks
//------------------------------------------------------------------------

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, Push, queue::LockedQueue<int>)(benchmark::State& state)
{
    for (auto _ : state)
        m_pQueue->enqueue(7);

    state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, Pop, queue::LockedQueue<int>)(benchmark::State& state)
{
    auto pops = 0;
    auto out = 0;

    for (auto _ : state)
        pops += m_pQueue->dequeue(out) ? 1 : 0;

    state.SetItemsProcessed(pops);
}

//------------------------------------------------------------------------
// Lockfree benchmarks
//------------------------------------------------------------------------

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, PushLF, queue::LockFreeQueue<int>)(benchmark::State& state)
{
    for (auto _ : state)
        m_pQueue->enqueue(7);

    state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE_DEFINE_F(QueueFixture, PopLF, queue::LockFreeQueue<int>)(benchmark::State& state)
{
    auto pops = 0;
    auto out = 0;

    if (state.threads == 2)
    {
        auto a = 0;
    }

    for (auto _ : state)
        pops += m_pQueue->dequeue(out) ? 1 : 0;

    state.SetItemsProcessed(pops);
}

BENCHMARK_REGISTER_F(QueueFixture, Push)->DenseThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, Pop)->DenseThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, PushLF)->DenseThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(QueueFixture, PopLF)->DenseThreadRange(1, 4)->UseRealTime();