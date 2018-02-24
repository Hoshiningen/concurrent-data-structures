#include "../queue/lockfree_queue.h"
#include "../queue/node.h"
#include "../../utility/memory.h"

#include <mutex>
#include <atomic>
#include <memory>

//==========================================================
// Locked Stack Implementation definitions
//==========================================================
struct queue::LockFreeQueue::Impl {
    Impl();
    ~Impl();

    // Prevent copying
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;

    void enqueue(int value);
    bool dequeue(int& out);

    std::atomic<queue::AtomicNode*> m_pHead;
    std::atomic<queue::AtomicNode*> m_pTail;
};

//==========================================================
// The default constructor for the Impl struct
//==========================================================
queue::LockFreeQueue::Impl::Impl() {
    // Assign a dummy node to the head and tail pointers
    auto node = new queue::AtomicNode{};
    m_pHead = node;
    m_pTail = node;
}

//==========================================================
// The destructor for the Impl class. Handles all memory
// cleanup
//==========================================================
queue::LockFreeQueue::Impl::~Impl() {
    // Frees any leftover nodes that weren't dequeued off
    auto pIter = m_pHead.load();
    while (pIter != nullptr)
    {
        // Retain a reference to the current top
        auto top = pIter;
        pIter = dynamic_cast<queue::AtomicNode*>(pIter->get_next());

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
void queue::LockFreeQueue::Impl::enqueue(int value) {
    queue::AtomicNode* node = new queue::AtomicNode{ value };
    queue::AtomicNode* tail = nullptr;
    queue::AtomicNode* next = nullptr;

    while (true) {
        // Repeatedly obtain the value of the tail and the next value
        tail = m_pTail;
        next = dynamic_cast<queue::AtomicNode*>(m_pTail.load()->get_next());

        // Ensure that the tail hasn't been changed 
        if (tail == m_pTail) {

            // If we aren't observing an intermediate enqueue result
            if (next == nullptr) {

                // Perform the CAS to set tail's next node with the new
                // node we're enqueueing
                if (tail->CASNext(next, node))
                    break;

            } else {

                // We're observing an intermediate result where the next pointer
                // was set, and so let us complete the operation
                std::atomic_compare_exchange_strong(&m_pTail, &tail, next);
            }
        }
    }

    // Lastly, point the tail at the enqueued node
    std::atomic_compare_exchange_strong(&m_pTail, &tail, node);
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
bool queue::LockFreeQueue::Impl::dequeue(int& out) {
    queue::AtomicNode* head = nullptr;
    queue::AtomicNode* tail = nullptr;
    queue::AtomicNode* next = nullptr;

    while (true) {
        head = m_pHead;
        tail = m_pTail;
        next = dynamic_cast<queue::AtomicNode*>(head->get_next());

        // Make sure that we're not observing an intermediate state
        if (head == m_pHead) {

            // Check to see if the tail is falling behind
            if (head == tail) {

                // Is the queue empty?
                if (next == nullptr)
                    return false;

                // Advance tail since it's falling behind
                std::atomic_compare_exchange_strong(&m_pTail, &tail, next);
            } else {

                // Obtain the value of the top of the queue and advance
                // the top to the next node
                out = next->get_value();
                if (std::atomic_compare_exchange_strong(&m_pHead, &head, next))
                    break;
            }
        }
    }

    // Free the previous top of the queue
    head->set_next(nullptr);
    delete head;
    head = nullptr;

    return true;
}

//==========================================================
// Locked Queue class definitions
//==========================================================

//==========================================================
// The default constructor for the LockFreeQueue class
//==========================================================
queue::LockFreeQueue::LockFreeQueue()
    : QueueBase(), m_pImpl(utility::make_unique<Impl>()) {}

//==========================================================
// Destructs the LockFreeQueue, freeing all allocated memory
//==========================================================
queue::LockFreeQueue::~LockFreeQueue() {
    // This automatically calls the dstor of Impl
}

//==========================================================
// Defines the move constructor
//
// \param other     The other queue to move into this one
//==========================================================
queue::LockFreeQueue::LockFreeQueue(LockFreeQueue && other) {
    m_pImpl = std::move(other.m_pImpl);
}

//==========================================================
// Defines the move assignment operator
//
// \param other     The other queue to move into this one
//==========================================================
queue::LockFreeQueue& queue::LockFreeQueue::operator=(LockFreeQueue && other) {
    if (this != &other)
        m_pImpl = std::move(other.m_pImpl);

    return *this;
}

//==========================================================
// This enqueues the specified value
//
// \param value   - The value to enqueue
//==========================================================
void queue::LockFreeQueue::enqueue(int value) {
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
bool queue::LockFreeQueue::dequeue(int& out) {
    return m_pImpl->dequeue(out);
}
