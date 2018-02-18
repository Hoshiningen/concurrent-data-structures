#pragma once

#include <memory>

namespace stack {

//==========================================================
// Represents an abstract base class for all stack
// implementations
//==========================================================
class StackBase {
public:
    virtual ~StackBase() { };

    virtual void push(int value) = 0;
    virtual bool pop(int& out) = 0;
};
}  // namespace stack
