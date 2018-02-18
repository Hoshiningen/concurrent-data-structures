#pragma once

#include "../stack/stack.h"

namespace stack {

//==========================================================
// This is an implementation of the Michael and Scott lock-
// based stack that controls access to the top node using a
// scoped lock
//==========================================================
class locked_stack : public stack::stack_base {
public:
    locked_stack();
    ~locked_stack();

    // Move operations
    locked_stack(locked_stack&& other);
    locked_stack& operator=(locked_stack&& other);

    // Prevent copying
    locked_stack(const locked_stack& other) = delete;
    locked_stack& operator=(const locked_stack& other) = delete;

    // inherited from stack::stack_base
    virtual void push(int value) override;
    virtual bool pop(int& out) override;

private:
    struct impl;
    std::unique_ptr<impl> m_pImpl;
};
}  // namespace stack
