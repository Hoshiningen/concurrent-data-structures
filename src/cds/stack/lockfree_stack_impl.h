#pragma once

#include "../stack/lockfree_stack.h"
#include "../stack/lockfree_node.h"
#include "../../utility/memory.h"

#include <atomic>
#include <memory>
#include <cassert>

//==========================================================
// Locked Stack implementation definitions
//==========================================================
template<typename T>
struct stack::LockFreeStack<T>::Impl {
    Impl();
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void push(T value);
    bool pop(T& out);

    std::atomic<stack::lf::NodePtr<T>> m_pTop;
};

//==========================================================
// The default constructor for the impl struct
//==========================================================
template<typename T>
stack::LockFreeStack<T>::Impl::Impl()
    : m_pTop{} {}

//==========================================================
// The destructor for the impl class. Handles all memory
// cleanup
//==========================================================
template<typename T>
stack::LockFreeStack<T>::Impl::~Impl() {
    auto pIter = m_pTop.load(std::memory_order_acquire);
    while (pIter.ptr != nullptr) {
        // Retain a reference to the current top
        auto top = pIter;
        pIter = pIter.ptr->next;

        // delete the previous top
        top.ptr->next.ptr = nullptr;
        delete top.ptr;
        top.ptr = nullptr;
    }
}

//==========================================================
// This pushes the specified value onto the stack
//
// \param value   - The value to push onto the stack
//==========================================================
template<typename T>
void stack::LockFreeStack<T>::Impl::push(T value) {
    auto node = new stack::lf::Node<T>{};
    node->value = value;

    stack::lf::NodePtr<T> top{};
    stack::lf::NodePtr<T> wrapper{};

    do
    {
        // Repeatedly try to set the new node as the new top
        // as long as the retrieved value and the current top
        // aren't equal.
        top = m_pTop;
        node->next.ptr = top.ptr;
        
        wrapper = stack::lf::NodePtr<T>{ node, top.count + 1 };
    } 
    while (!m_pTop.compare_exchange_weak(top, wrapper));
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
bool stack::LockFreeStack<T>::Impl::pop(T& out) {
    stack::lf::NodePtr<T> top{};

    do
    {
        // Repeatedly try to obtain the top node until they're equal,
        // upon which replace the top node with the next one in the list
        top = m_pTop;

        if (top.ptr == nullptr)
            return false;

        auto wrapper = stack::lf::NodePtr<T>{
            top.ptr->next.ptr, top.count + 1 };
    }
    while (!m_pTop.compare_exchange_weak(top, wrapper));

    // Obtain the old top's value and pass it out
    out = top.ptr->value;

    // Delete the old top
    top.ptr->next.ptr = nullptr;
    delete top.ptr;
    top.ptr = nullptr;

    return true;
}

//==========================================================
// Locked Stack class definitions
//==========================================================

//==========================================================
// The default constructor for the lockfree_stack class
//==========================================================
template<typename T>
stack::LockFreeStack<T>::LockFreeStack()
    : m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the lockfree_stack, freeing all allocated memory
//==========================================================
template<typename T>
stack::LockFreeStack<T>::~LockFreeStack() {
    // This automatically calls the dstor of impl
}

//==========================================================
// This defines the move assignment operator
//
// \param other   - The value to move into this one
//==========================================================
template<typename T>
stack::LockFreeStack<T>& stack::LockFreeStack<T>::operator=(LockFreeStack&& other) {
    if (this != &other)
        m_pImpl = std::move(other.m_pImpl);

    return *this;
}

//==========================================================
// This defines the move constructor
//
// \param other   - The value to move into this one
//==========================================================
template<typename T>
stack::LockFreeStack<T>::LockFreeStack(LockFreeStack && other)
    : m_pImpl{ std::move(other.m_pImpl) }
{}

//==========================================================
// This pushes the specified value onto the stack
//
// \param value   - The value to push onto the stack
//==========================================================
template<typename T>
void stack::LockFreeStack<T>::push(T value) {
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
bool stack::LockFreeStack<T>::pop(T& out) {
    return m_pImpl->pop(out);
}
