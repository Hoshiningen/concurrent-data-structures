#pragma once

#include <memory>

namespace stack {

////////////////////////////////////////////////////////////
//
// Represents an abstract base class for all stack
// implementations
//
////////////////////////////////////////////////////////////
class stack_base {
public:
    virtual ~stack_base() { };

    virtual void push(int value) = 0;
    virtual bool pop(int& out) = 0;
};
}  // namespace stack
