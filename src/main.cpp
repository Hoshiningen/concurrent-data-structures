#include <iostream>
#include <thread>

#include "cds/stack/locked_stack.h"
#include "cds/stack/lockfree_stack.h"

void produce(stack::stack_base* stack, const int& n)
{ 
    for (auto i = 0; i < n; ++i)
        stack->push(i);
}

void consume(stack::stack_base* stack)
{
    auto top_val = 0;
    while (stack->pop(top_val))
        std::cout << top_val << "\n";
}

int main()
{
    auto stack = new stack::lockfree_stack{};

    auto t1 = std::thread{ produce, stack, 20 };
    auto t2 = std::thread{ produce, stack, 20 };
    auto t3 = std::thread{ consume, stack };

    t1.join();
    t2.join();
    t3.join();

    std::cin.get();
}
