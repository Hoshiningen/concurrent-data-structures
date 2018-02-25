#pragma once

#include "../stack/stack.h"

namespace stack {

//==========================================================
// This is an implementation of the Michael and Scott lock-
// based stack that controls access to the top node using a
// scoped lock
//==========================================================
template<typename T>
class LockedStack : public stack::StackBase<T> {
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
    virtual void push(T value) override;
    virtual bool pop(T& out) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
}  // namespace stack

#include "../stack/locked_stack_impl.h"