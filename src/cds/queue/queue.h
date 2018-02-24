#pragma once

namespace queue {

//==========================================================
// Represents an abstract base class for all stack
// implementations
//==========================================================
class QueueBase {
public:
    virtual ~QueueBase() { };

    virtual void enqueue(int value) = 0;
    virtual bool dequeue(int& out) = 0;
};
}  // namespace stack
