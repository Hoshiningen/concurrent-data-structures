#include "../stack/lockfree_stack.h"
#include "../stack/node.h"
#include "../../utility/memory.h"

#include <atomic>
#include <memory>

//==========================================================
// Locked Stack implementation definitions
//==========================================================
struct stack::LockFreeStack::Impl {
    Impl();
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void push(int value);
    bool pop(int& out);

    std::atomic<stack::NodeBase*> m_pTop;
};

//==========================================================
// The default constructor for the impl struct
//==========================================================
stack::LockFreeStack::Impl::Impl()
    : m_pTop() {}

//==========================================================
// The destructor for the impl class. Handles all memory
// cleanup
//==========================================================
stack::LockFreeStack::Impl::~Impl() {
    // Need to investigate the safety of this..
    auto pIter = m_pTop.load();
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
void stack::LockFreeStack::Impl::push(int value) {
    stack::NodeBase* node = new stack::AccessCountNode(value);
    stack::NodeBase* top = nullptr;

    do
    {
        // Repeatedly try to set the new node as the new top
        // as long as the retrieved value and the current top
        // aren't equal.
        top = m_pTop;
        node->set_next(top);
    } 
    while (!std::atomic_compare_exchange_weak(&m_pTop, &top, node));
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
bool stack::LockFreeStack::Impl::pop(int& out) {
    stack::NodeBase* top = nullptr;

    do
    {
        // Repeatedly try to obtain the top node until they're equal,
        // upon which replace the top node with the next one in the list
        top = m_pTop;

        if (top == nullptr)
            return false;
    }
    while (!std::atomic_compare_exchange_weak(&m_pTop, &top, top->get_next()));

    // Obtain the old top's value
    out = top->get_value();

    // Delete the old top
    top->set_next(nullptr);
    delete top;
    top = nullptr;

    return true;
}

//==========================================================
// Locked Stack class definitions
//==========================================================

//==========================================================
// The default constructor for the lockfree_stack class
//==========================================================
stack::LockFreeStack::LockFreeStack()
    : StackBase(), m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the lockfree_stack, freeing all allocated memory
//==========================================================
stack::LockFreeStack::~LockFreeStack() {
    // This automatically calls the dstor of impl
}

//==========================================================
// This defines the move assignment operator
//
// \param other   - The value to move into this one
//==========================================================
stack::LockFreeStack& stack::LockFreeStack::operator=(LockFreeStack&& other) {
    if (this != &other)
        m_pImpl = std::move(other.m_pImpl);

    return *this;
}

//==========================================================
// This defines the move constructor
//
// \param other   - The value to move into this one
//==========================================================
stack::LockFreeStack::LockFreeStack(LockFreeStack && other)
    : m_pImpl{ std::move(other.m_pImpl) }
{}

//==========================================================
// This pushes the specified value onto the stack
//
// \param value   - The value to push onto the stack
//==========================================================
void stack::LockFreeStack::push(int value) {
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
bool stack::LockFreeStack::pop(int& out) {
    return m_pImpl->pop(out);
}
