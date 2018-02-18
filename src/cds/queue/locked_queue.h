#pragma once

#include "../queue/queue.h"

#include <memory>

namespace queue {

//==========================================================
// Represents an implementation of the Michael and Scott
// lock based queue
//==========================================================
class LockedQueue : public queue::QueueBase {
public:
    LockedQueue();
    ~LockedQueue();

    // Move operations
    LockedQueue(LockedQueue&& other);
    LockedQueue& operator=(LockedQueue&& other);

    // Prevent copying
    LockedQueue(const LockedQueue& other) = delete;
    LockedQueue& operator=(const LockedQueue& other) = delete;

    // inherited from stack::stack_base
    virtual void enqueue(int value) override;
    virtual bool dequeue(int& out) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
}  // namespace queue