#pragma once

#include "../queue/lockfree_queue.h"
#include "../queue/lockfree_node.h"
#include "../../utility/memory.h"

#include <mutex>
#include <atomic>
#include <memory>
#include <cassert>

//==========================================================
// Locked Stack Implementation definitions
//==========================================================
template<typename T>
struct queue::LockFreeQueue<T>::Impl {
    Impl();
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void enqueue(T value);
    bool dequeue(T& out);

    std::atomic<queue::lf::NodePtr<T>> m_pHead;
    std::atomic<queue::lf::NodePtr<T>> m_pTail;
};

//==========================================================
// The default constructor for the Impl struct
//==========================================================
template<typename T>
queue::LockFreeQueue<T>::Impl::Impl() {
    assert((std::is_trivially_constructible<queue::lf::NodePtr<T>>::value));
    assert((std::is_trivially_copy_constructible<queue::lf::NodePtr<T>>::value));
    assert((std::is_trivially_move_constructible<queue::lf::NodePtr<T>>::value));
    assert((std::is_trivially_copy_assignable<queue::lf::NodePtr<T>>::value));
    assert((std::is_trivially_move_assignable<queue::lf::NodePtr<T>>::value));

    // Assign a dummy node to the head and tail pointers
    auto wrapper = queue::lf::NodePtr<T>{};
    wrapper.ptr = new queue::lf::Node<T>{};

    m_pHead = wrapper;
    m_pTail = wrapper;
}

//==========================================================
// The destructor for the Impl class. Handles all memory
// cleanup
//==========================================================
template<typename T>
queue::LockFreeQueue<T>::Impl::~Impl() {
    auto pIter = m_pHead.load(std::memory_order_acquire);
    while (pIter.ptr != nullptr) {
        // Retain a reference to the current top
        auto top = pIter;
        pIter = pIter.ptr->next;

        // delete the previous top
        top.ptr->next.store({ nullptr, 0 }, std::memory_order_release);
        delete top.ptr;
        top.ptr = nullptr;
    }
}

//==========================================================
// This enqueues the specified value
//
// \param value   - The value to enqueue
//==========================================================
template<typename T>
void queue::LockFreeQueue<T>::Impl::enqueue(T value) {
    queue::lf::Node<T>* node = new queue::lf::Node<T>{ value };
    queue::lf::NodePtr<T> tail{};
    queue::lf::NodePtr<T> next{};
    queue::lf::NodePtr<T> wrapper{};

    while (true) {
        // Repeatedly obtain the value of the tail and the next value
        tail = m_pTail;
        next = tail.ptr->next;

        // Ensure that the tail hasn't been changed 
        if (tail == m_pTail.load(std::memory_order_acquire)) {

            // If we aren't observing an intermediate enqueue result
            if (next.ptr == nullptr) {

                wrapper.ptr = node;
                wrapper.count = next.count + 1;

                // Perform the CAS to set tail's next node with the new
                // node we're enqueueing
                if (std::atomic_compare_exchange_strong(&tail.ptr->next, &next, wrapper))
                    break;

            }
            else {

                wrapper.ptr = next.ptr;
                wrapper.count = tail.count + 1;

                // We're observing an intermediate result where the next pointer
                // was set, and so let us complete the operation
                std::atomic_compare_exchange_strong(&m_pTail, &tail, wrapper);
            }
        }
    }

    wrapper.ptr = node;
    wrapper.count = tail.count + 1;

    // Lastly, point the tail at the enqueued node
    std::atomic_compare_exchange_strong(&m_pTail, &tail, wrapper);
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
bool queue::LockFreeQueue<T>::Impl::dequeue(T& out) {
    queue::lf::NodePtr<T> head{};
    queue::lf::NodePtr<T> tail{};
    queue::lf::NodePtr<T> next{};
    queue::lf::NodePtr<T> wrapper{};

    while (true) {
        head = m_pHead;
        tail = m_pTail;
        next = head.ptr->next;

        // Make sure that we're not observing an intermediate state
        if (head == m_pHead.load(std::memory_order_acquire)) {

            // Check to see if the tail is falling behind
            if (head.ptr == tail.ptr) {

                // Is the queue empty?
                if (next.ptr == nullptr)
                    return false;

                wrapper.ptr = next.ptr;
                wrapper.count = tail.count + 1;

                // Advance tail since it's falling behind
                std::atomic_compare_exchange_strong(&m_pTail, &tail, wrapper);
            }
            else {

                // Obtain the value of the top of the queue and advance
                // the top to the next node
                out = next.ptr->value;

                wrapper.ptr = next.ptr;
                wrapper.count = head.count + 1;

                if (std::atomic_compare_exchange_strong(&m_pHead, &head, wrapper))
                    break;
            }
        }
    }

    // delete the previous head
    head.ptr->next.store({ nullptr, 0 }, std::memory_order_release);
    delete head.ptr;
    head.ptr = nullptr;

    return true;
}

//==========================================================
// Locked Queue class definitions
//==========================================================

//==========================================================
// The default constructor for the LockFreeQueue class
//==========================================================
template<typename T>
queue::LockFreeQueue<T>::LockFreeQueue()
    : QueueBase(), m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the LockFreeQueue, freeing all allocated memory
//==========================================================
template<typename T>
queue::LockFreeQueue<T>::~LockFreeQueue() {
    // This automatically calls the dstor of Impl
}

//==========================================================
// Defines the move constructor
//
// \param other     The other queue to move into this one
//==========================================================
template<typename T>
queue::LockFreeQueue<T>::LockFreeQueue(LockFreeQueue && other) {
    m_pImpl = std::move(other.m_pImpl);
}

//==========================================================
// Defines the move assignment operator
//
// \param other     The other queue to move into this one
//==========================================================
template<typename T>
queue::LockFreeQueue<T>& queue::LockFreeQueue<T>::operator=(LockFreeQueue && other) {
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
void queue::LockFreeQueue<T>::enqueue(T value) {
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
bool queue::LockFreeQueue<T>::dequeue(T& out) {
    return m_pImpl->dequeue(out);
}
