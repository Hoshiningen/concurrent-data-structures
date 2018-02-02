#include <iostream>
#include <thread>

#include "cds/locked_stack.h"

void produce(stack::locked_stack* stack, const int& n) {
    auto i = 0;
    for (; i < n; ++i)
        stack->push(i);
}

void consume(stack::locked_stack* stack) {
    auto top_val = 0;
    while (stack->pop(top_val))
        std::cout << top_val << "\n";
}

int main() {
    auto a = new stack::locked_stack{};

    auto t1 = std::thread{ produce, a, 10 };
    auto t2 = std::thread{ consume, a };

    t1.join();
    t2.join();

    std::cin.get();
}
