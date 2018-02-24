#pragma once

#include "../stack/stack.h"

namespace stack {

//==========================================================
// This is an implementation of the Michael and Scott lock-
// based stack that controls access to the top node using a
// scoped lock
//==========================================================
class LockedStack : public stack::StackBase {
public:
    LockedStack();
    ~LockedStack();

    // Move operations
    LockedStack(LockedStack&& other);
    LockedStack& operator=(LockedStack&& other);

    // Prevent copying
    LockedStack(const LockedStack& other) = delete;
    LockedStack& operator=(const LockedStack& other) = delete;

    // inherited from stack::stack_base
    virtual void push(int value) override;
    virtual bool pop(int& out) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
}  // namespace stack
