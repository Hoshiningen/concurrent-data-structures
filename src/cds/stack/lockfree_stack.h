#pragma once

#include "../stack/stack.h"

namespace stack {

    class lockfree_stack : public stack::stack_base {
    public:
        lockfree_stack();
        ~lockfree_stack();

        // Prevent copying
        lockfree_stack(const lockfree_stack& other) = delete;
        lockfree_stack& operator=(const lockfree_stack& other) = delete;

        // inherited from stack::stack_base
        virtual void push(int value) override;
        virtual bool pop(int& out) override;

    private:
        struct impl;
        std::unique_ptr<impl> const m_pImpl;
    };
}  // namespace stack