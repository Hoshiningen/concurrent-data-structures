#pragma once

#include "../stack/locked_stack.h"
#include "../../utility/node.h"
#include "../../utility/memory.h"

#include <memory>
#include <mutex>

//==========================================================
// Locked Stack implementation definitions
//==========================================================
template<typename T>
struct stack::LockedStack<T>::Impl {
    Impl() = default;
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void push(T value);
    bool pop(T& out);

    utility::NodeBase<T>* m_pTop;

    mutable std::mutex mTopMut;
};

//==========================================================
// The destructor for the impl class. Handles all memory
// cleanup
//==========================================================
template<typename T>
stack::LockedStack<T>::Impl::~Impl() {
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
template<typename T>
void stack::LockedStack<T>::Impl::push(T value) {
    std::lock_guard<std::mutex> lock{ mTopMut };

    auto node = new utility::Node<T>{ value };

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
template<typename T>
bool stack::LockedStack<T>::Impl::pop(T& out) {
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
template<typename T>
stack::LockedStack<T>::LockedStack()
    : StackBase(), m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the locked_stack, freeing all allocated memory
//==========================================================
template<typename T>
stack::LockedStack<T>::~LockedStack() {
    // This automatically calls the dstor of impl
}

//==========================================================
// This defines the move assignment operator
//
// \param other   - Another stack to move into this one
//==========================================================
template<typename T>
stack::LockedStack<T>& stack::LockedStack<T>::operator=(LockedStack&& other) {
    if (this != &other)
        m_pImpl = std::move(other.m_pImpl);

    return *this;
}

//==========================================================
// This defines the move constructor
//
// \param other   - Another stack to move into this one
//==========================================================
template<typename T>
stack::LockedStack<T>::LockedStack(LockedStack&& other)
    : m_pImpl{std::move(other.m_pImpl)}
{}

//==========================================================
// This pushes the specified value onto the stack
//
// \param value   - The value to push onto the stack
//==========================================================
template<typename T>
void stack::LockedStack<T>::push(T value) {
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
template<typename T>
bool stack::LockedStack<T>::pop(T& out) {
    return m_pImpl->pop(out);
}
