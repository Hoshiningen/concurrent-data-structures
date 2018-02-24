#include "../queue/locked_queue.h"
#include "../queue/node.h"
#include "../../utility/memory.h"

#include <mutex>
#include <memory>

//==========================================================
// Locked Stack implementation definitions
//==========================================================
struct queue::LockedQueue::Impl {
    Impl();
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void enqueue(int value);
    bool dequeue(int& out);

    queue::NodeBase* m_pHead;
    queue::NodeBase* m_pTail;

    mutable std::mutex m_HeadMut;
    mutable std::mutex m_TailMut;
};

//==========================================================
// The default constructor for the impl struct
//==========================================================
queue::LockedQueue::Impl::Impl() 
    : m_pHead(nullptr), m_pTail(nullptr) {}

//==========================================================
// The destructor for the impl class. Handles all memory
// cleanup
//==========================================================
queue::LockedQueue::Impl::~Impl() {
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
void queue::LockedQueue::Impl::enqueue(int value) {
    std::lock_guard<std::mutex> lock{ m_TailMut };

    auto node = new queue::AtomicNode{ value };

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
bool queue::LockedQueue::Impl::dequeue(int& out) {
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
queue::LockedQueue::LockedQueue()
    : QueueBase(), m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the locked_queue, freeing all allocated memory
//==========================================================
queue::LockedQueue::~LockedQueue() {
    // This automatically calls the dstor of impl
}

//==========================================================
// Defines the move constructor
//
// \param other     The other queue to move into this one
//==========================================================
queue::LockedQueue::LockedQueue(LockedQueue && other) {
    m_pImpl = std::move(other.m_pImpl);
}

//==========================================================
// Defines the move assignment operator
//
// \param other     The other queue to move into this one
//==========================================================
queue::LockedQueue& queue::LockedQueue::operator=(LockedQueue && other) {
    if (this != &other)
        m_pImpl = std::move(other.m_pImpl);

    return *this;
}

//==========================================================
// This enqueues the specified value
//
// \param value   - The value to enqueue
//==========================================================
void queue::LockedQueue::enqueue(int value) {
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
bool queue::LockedQueue::dequeue(int& out) {
    return m_pImpl->dequeue(out);
}
