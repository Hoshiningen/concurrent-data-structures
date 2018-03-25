#include <iostream>
#include <thread>

#include "cds/stack/locked_stack.h"
#include "cds/stack/lockfree_stack.h"
#include "cds/queue/locked_queue.h"
#include "cds/queue/lockfree_queue.h"

void produceS(stack::StackBase<float>* stack, const float& n)
{ 
    for (auto i = 1; i <= n; ++i)
        stack->push(i + .5f);
}

void consumeS(stack::StackBase<float>* stack)
{
    float top_val = 0.0;
    while (stack->pop(top_val))
        std::cout << top_val << "\n";
}

void produceQ(queue::QueueBase<float>* queue, const float& n)
{
    for (auto i = -5; i <= n; ++i)
        queue->enqueue(i * .7);
}

void consumeQ(queue::QueueBase<float>* queue)
{
    float top_val = 0.0f;
    while (queue->dequeue(top_val))
        std::cout << top_val << "\n";
}

int main()
{
    auto stack = new stack::LockedStack<float>{};

    auto t1 = std::thread{ produceS, stack, 10 };
    auto t3 = std::thread{ consumeS, stack };

    t1.join();
    t3.join();

    std::cout << "\n\n";

    auto queue = new queue::LockedQueue<float>{};

    auto t4 = std::thread{ produceQ, queue, 10 };
    auto t6 = std::thread{ consumeQ, queue };

    t4.join();
    t6.join();

    delete stack;
    delete queue;

    std::cin.get();
}
