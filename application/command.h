#pragma once

#include <thread>
#include <chrono>

namespace application {
namespace pc {

class RandomComputationCommand
{
public:
    void execute(std::chrono::nanoseconds time) const noexcept
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto end = start + time;

        do 
        {
            std::this_thread::yield();
        } 
        while (std::chrono::high_resolution_clock::now() < end);
    }
};

}  // namespace pc
}  // namespace application
