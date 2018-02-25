#pragma once

#include <memory>

namespace stack {

//==========================================================
// Represents an abstract base class for all stack
// implementations
//==========================================================
template<typename T>
class StackBase {
public:
    virtual ~StackBase() { };

    virtual void push(T value) = 0;
    virtual bool pop(T& out) = 0;
};
}  // namespace stack
