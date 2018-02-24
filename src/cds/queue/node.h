#pragma once

#include <atomic>

namespace queue {

//==========================================================
// Represents an abstract base class for all node
// implementations
//==========================================================
class NodeBase {
public:

    virtual ~NodeBase() { }

    virtual void set_value(int value) = 0;
    virtual int get_value() = 0;

    virtual void set_next(NodeBase* next) = 0;
    virtual NodeBase* get_next() = 0;
};

//==========================================================
// Represents a node that stores an atomic next pointer
//==========================================================
class AtomicNode : public NodeBase {
public:
    AtomicNode()
        : m_value(0), m_pNext(nullptr) {}

    explicit AtomicNode(int value)
        : m_value(value), m_pNext(nullptr) {}

    ~AtomicNode() {}

    virtual void set_value(int value) override { m_value = value; }
    virtual void set_next(NodeBase* next) override {
        m_pNext.store(dynamic_cast<AtomicNode*>(next));
    }

    virtual int get_value() override { return m_value; }
    virtual NodeBase* get_next() override { return m_pNext; }

    virtual bool CASNext(AtomicNode* expected, AtomicNode* desired) {
        return std::atomic_compare_exchange_strong(&m_pNext, &expected, desired);
    }

private:
    int m_value;
    std::atomic<AtomicNode*> m_pNext;
};
} // namespace stack
