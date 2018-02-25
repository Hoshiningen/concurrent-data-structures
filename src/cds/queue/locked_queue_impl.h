#pragma once

#include "../queue/locked_queue.h"
#include "../../utility/node.h"
#include "../../utility/memory.h"

#include <mutex>
#include <memory>

//==========================================================
// Locked Stack implementation definitions
//==========================================================
template<typename T>
struct queue::LockedQueue<T>::Impl {
    Impl() = default;
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void enqueue(T value);
    bool dequeue(T& out);

    utility::NodeBase<T>* m_pHead;
    utility::NodeBase<T>* m_pTail;

    mutable std::mutex m_HeadMut;
    mutable std::mutex m_TailMut;
};

//==========================================================
// The destructor for the impl class. Handles all memory
// cleanup
//==========================================================
template<typename T>
queue::LockedQueue<T>::Impl::~Impl() {
    // Delete any remaining nodes that weren't dequeued
    auto pIter = m_pHead;
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
// This enqueues the specified value
//
// \param value   - The value to enqueue
//==========================================================
template<typename T>
void queue::LockedQueue<T>::Impl::enqueue(T value) {
    std::lock_guard<std::mutex> lock{ m_TailMut };

    auto node = new utility::Node<T>{ value };

    if (m_pTail == nullptr) {
        m_pHead = node;
        m_pTail = node;
    } else {
        m_pTail->set_next(node);

        if (m_pHead == m_pTail)
            m_pHead->set_next(node);
        
        m_pTail = node;
    }
}

//==========================================================
// This attempts to perform a dequeue operation, which puts
// the top value into \param{out}, and returns true if the
// operation was successful. If the stack is empty, then it
// returns false.
//
// \param out   - An output variable that is assigned the
//                value that was at the top of the stack
//
// \return      - The success of the pop operation
//==========================================================
template<typename T>
bool queue::LockedQueue<T>::Impl::dequeue(T& out) {
    std::lock_guard<std::mutex> lock{ m_HeadMut };

    if (m_pHead == nullptr)
        return false;

    // Retain a temp ref to the old top
    auto top = m_pHead;
    out = top->get_value();

    // set the new top
    m_pHead = m_pHead->get_next();

    // delete the old top
    top->set_next(nullptr);
    delete top;
    top = nullptr;

    return true;
}

//==========================================================
// Locked Queue class definitions
//==========================================================

//==========================================================
// The default constructor for the locked_queue class
//==========================================================
template<typename T>
queue::LockedQueue<T>::LockedQueue()
    : QueueBase(), m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the locked_queue, freeing all allocated memory
//==========================================================
template<typename T>
queue::LockedQueue<T>::~LockedQueue() {
    // This automatically calls the dstor of impl
}

//==========================================================
// Defines the move constructor
//
// \param other     The other queue to move into this one
//==========================================================
template<typename T>
queue::LockedQueue<T>::LockedQueue(LockedQueue && other) {
    m_pImpl = std::move(other.m_pImpl);
}

//==========================================================
// Defines the move assignment operator
//
// \param other     The other queue to move into this one
//==========================================================
template<typename T>
queue::LockedQueue<T>& queue::LockedQueue<T>::operator=(LockedQueue<T> && other) {
    if (this != &other)
        m_pImpl = std::move(other.m_pImpl);

    return *this;
}

//==========================================================
// This enqueues the specified value
//
// \param value   - The value to enqueue
//==========================================================
template<typename T>
void queue::LockedQueue<T>::enqueue(T value) {
    m_pImpl->enqueue(value);
}

//==========================================================
// This attempts to perform a dequeue operation, which puts
// the top value into \param{out}, and returns true if the
// operation was successful. If the stack is empty, then it
// returns false.
//
// \param out   - An output variable that is assigned the
//                value that was at the top of the stack
//
// \return      - The success of the pop operation
//==========================================================
template<typename T>
bool queue::LockedQueue<T>::dequeue(T& out) {
    return m_pImpl->dequeue(out);
}
