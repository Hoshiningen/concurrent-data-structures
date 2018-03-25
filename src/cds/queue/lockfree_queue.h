#pragma once

#include "../queue/queue.h"

#include <memory>

namespace queue {

//==========================================================
// Represents an implementation of the lock-free queue described
// in the paper by Michael and Scott
//==========================================================
template<typename T>
class LockFreeQueue : public queue::QueueBase<T> {
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
    virtual void enqueue(T value) override;
    virtual bool dequeue(T& out) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
}  // namespace queue

#include "../queue/lockfree_queue_impl.h"