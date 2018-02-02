#pragma once

#include "../cds/stack.h"

namespace stack {

class locked_stack : public stack::stack_base {
public:
    locked_stack();
    ~locked_stack();

    // Prevent copying
    locked_stack(const locked_stack& other) = delete;
    locked_stack& operator=(const locked_stack& other) = delete;

    // inherited from stack::stack_base
    void push(int value) override;
    bool pop(int& out) override;

private:
    struct impl;
    std::unique_ptr<impl> const m_pImpl;
};
}  // namespace stack
