#pragma once

#include "../stack/stack.h"

namespace stack {

//==========================================================
// This is an implementation of the Trieber Stack, which is
// a lock-free stack algorithm that utilizes compare and swap
// to atomically swap the top node during pushes and pops
//==========================================================
class lockfree_stack : public stack::stack_base {
public:
    lockfree_stack();
    ~lockfree_stack();

    // Move operations
    lockfree_stack(lockfree_stack&& other);
    lockfree_stack& operator=(lockfree_stack&& other);

    // Prevent copying
    lockfree_stack(const lockfree_stack& other) = delete;
    lockfree_stack& operator=(const lockfree_stack& other) = delete;

    // inherited from stack::stack_base
    virtual void push(int value) override;
    virtual bool pop(int& out) override;

private:
    struct impl;
    std::unique_ptr<impl> m_pImpl;
};
}  // namespace stack
