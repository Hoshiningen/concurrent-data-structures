#pragma once

#include "../queue/queue.h"

#include <memory>

namespace queue {

//==========================================================
// Represents an implementation of the Michael and Scott
// lock based queue
//==========================================================
class LockFreeQueue : public queue::QueueBase {
public:
    LockFreeQueue();
    ~LockFreeQueue();

    // Move operations
    LockFreeQueue(LockFreeQueue&& other);
    LockFreeQueue& operator=(LockFreeQueue&& other);

    // Prevent copying
    LockFreeQueue(const LockFreeQueue& other) = delete;
    LockFreeQueue& operator=(const LockFreeQueue& other) = delete;

    // inherited from stack::stack_base
    virtual void enqueue(int value) override;
    virtual bool dequeue(int& out) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
}  // namespace queue
