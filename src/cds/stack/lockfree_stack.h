#pragma once

#include "../stack/stack.h"

namespace stack {

//==========================================================
// This is an implementation of the Trieber Stack, which is
// a lock-free stack algorithm that utilizes compare and swap
// to atomically swap the top node during pushes and pops
//==========================================================
class LockFreeStack : public stack::StackBase {
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
    virtual void push(int value) override;
    virtual bool pop(int& out) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
}  // namespace stack
