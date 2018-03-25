#pragma once

namespace queue {

//==========================================================
// Represents an abstract base class for all stack
// implementations
//==========================================================
template<typename T>
class QueueBase {
public:
    virtual ~QueueBase() { };

    virtual void enqueue(T value) = 0;
    virtual bool dequeue(T& out) = 0;
};
}  // namespace stack
