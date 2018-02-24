#include "../stack/locked_stack.h"
#include "../stack/node.h"
#include "../../utility/memory.h"

#include <memory>
#include <mutex>

//==========================================================
// Locked Stack implementation definitions
//==========================================================
struct stack::LockedStack::Impl {
    Impl();
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void push(int value);
    bool pop(int& out);

    stack::NodeBase* m_pTop;

    mutable std::mutex mTopMut;
};

//==========================================================
// The default constructor for the impl struct
//==========================================================
stack::LockedStack::Impl::Impl()
    : m_pTop(nullptr) {}

//==========================================================
// The destructor for the impl class. Handles all memory
// cleanup
//==========================================================
stack::LockedStack::Impl::~Impl() {
    // Need to investigate the safety of this..
    auto pIter = m_pTop;
    while (pIter != nullptr) {
        // Retain a reference to the current top
        auto top = pIter;
        pIter = pIter->get_next();

        // delete the previous top
        top->set_next(nullptr);
        delete top;
        top = nullptr;
    }
}

//==========================================================
// This pushes the specified value onto the stack
//
// \param value   - The value to push onto the stack
//==========================================================
void stack::LockedStack::Impl::push(int value) {
    std::lock_guard<std::mutex> lock{ mTopMut };

    auto node = new stack::Node{ value };

    if (m_pTop == nullptr) {
        m_pTop = node;
    } else {
        node->set_next(m_pTop);
        m_pTop = node;
    }
}

//==========================================================
// This attempts to pop the stack, which puts the top value
// into \param{out}, and returns true if the operation was
// successful. If the stack is empty, then it returns false.
//
// \param out   - An output variable that is assigned the
//                value that was at the top of the stack
//
// \return      - The success of the pop operation
//==========================================================
bool stack::LockedStack::Impl::pop(int& out) {
    std::lock_guard<std::mutex> lock{ mTopMut };

    if (m_pTop == nullptr)
        return false;

    // Retain a temp ref to the old top
    auto top = m_pTop;
    out = top->get_value();

    // set the new top
    m_pTop = m_pTop->get_next();

    // delete the old top
    top->set_next(nullptr);
    delete top;
    top = nullptr;

    return true;
}

//==========================================================
// Locked Stack class definitions
//==========================================================


//==========================================================
// The default constructor for the locked_stack class
//==========================================================
stack::LockedStack::LockedStack()
    : StackBase(), m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the locked_stack, freeing all allocated memory
//==========================================================
stack::LockedStack::~LockedStack() {
    // This automatically calls the dstor of impl
}

//==========================================================
// This defines the move assignment operator
//
// \param other   - Another stack to move into this one
//==========================================================
stack::LockedStack& stack::LockedStack::operator=(LockedStack&& other) {
    if (this != &other)
        m_pImpl = std::move(other.m_pImpl);

    return *this;
}

//==========================================================
// This defines the move constructor
//
// \param other   - Another stack to move into this one
//==========================================================
stack::LockedStack::LockedStack(LockedStack&& other)
    : m_pImpl{std::move(other.m_pImpl)}
{}

//==========================================================
// This pushes the specified value onto the stack
//
// \param value   - The value to push onto the stack
//==========================================================
void stack::LockedStack::push(int value) {
    m_pImpl->push(value);
}

//==========================================================
// This attempts to pop the stack, which puts the top value
// into \param{out}, and returns true if the operation was
// successful. If the stack is empty, then it returns false.
//
// \param out   - An output variable that is assigned the
//                value that was at the top of the stack
//
// \return      - The success of the pop operation
//==========================================================
bool stack::LockedStack::pop(int& out) {
    return m_pImpl->pop(out);
}
