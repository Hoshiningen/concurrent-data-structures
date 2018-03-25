#pragma once

#include "../stack/stack.h"

namespace stack {

//==========================================================
// This is an implementation of the Trieber Stack, which is
// a lock-free stack algorithm that utilizes compare and swap
// to atomically swap the top node during pushes and pops
//==========================================================
template<typename T>
class LockFreeStack : public stack::StackBase<T> {
public:
    LockFreeStack();
    ~LockFreeStack();

    // Move operations
    LockFreeStack(LockFreeStack&& other);
    LockFreeStack& operator=(LockFreeStack&& other);

    // Prevent copying
    LockFreeStack(const LockFreeStack& other) = delete;
    LockFreeStack& operator=(const LockFreeStack& other) = delete;

    // inherited from stack::stack_base
    virtual void push(T value) override;
    virtual bool pop(T& out) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
}  // namespace stack

#include "../stack/lockfree_stack_impl.h"