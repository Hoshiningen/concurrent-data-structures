#pragma once

namespace stack {

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
// Represents a stack node that keeps track of access counts
//==========================================================
class AccessCountNode : public NodeBase {
public:
    explicit AccessCountNode(int value)
        : m_pNext(nullptr), m_value(value), m_count(0) { }

    ~AccessCountNode() {
        delete m_pNext;
        m_pNext = nullptr;
    }

    virtual void set_value(int value) override { m_value = value; }
    virtual int get_value() override { return m_value; }

    virtual void set_next(NodeBase* next) override { m_pNext = std::move(next); }
    virtual NodeBase* get_next() override { return m_pNext; }

private:
    NodeBase* m_pNext;
    int m_value;
    unsigned int m_count;
};

//==========================================================
// Represents a standard stack node
//==========================================================
class Node : public NodeBase {
public:
    Node()
        : m_value(0), m_pNext(nullptr) {}

    explicit Node(int value)
        : m_value(value), m_pNext(nullptr) {}

    ~Node() {
        delete m_pNext;
        m_pNext = nullptr;
    }

    virtual void set_value(int value) override { m_value = value; }
    virtual void set_next(NodeBase* next) override { m_pNext = next; }

    virtual int get_value() override { return m_value; }
    virtual NodeBase* get_next() override { return m_pNext; }

private:
    int m_value;
    NodeBase* m_pNext;
};
} // namespace stack
