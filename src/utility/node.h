#pragma once

namespace utility {

//==========================================================
// Represents an abstract base class for all node
// implementations
//==========================================================
template<typename T>
class NodeBase {
public:

    virtual ~NodeBase() { }

    virtual void set_value(T value) = 0;
    virtual T get_value() = 0;

    virtual void set_next(NodeBase<T>* next) = 0;
    virtual NodeBase<T>* get_next() = 0;
};

//==========================================================
// Represents a standard stack node
//==========================================================
template<typename T>
class Node : public NodeBase<T> {
public:
    Node()
        : m_value(default(T)), m_pNext(nullptr) {}

    explicit Node(T value)
        : m_value(value), m_pNext(nullptr) {}

    ~Node() {
        delete m_pNext;
        m_pNext = nullptr;
    }

    virtual void set_value(T value) override { m_value = value; }
    virtual void set_next(NodeBase<T>* next) override { m_pNext = next; }

    virtual T get_value() override { return m_value; }
    virtual NodeBase<T>* get_next() override { return m_pNext; }

private:
    T m_value;
    NodeBase<T>* m_pNext;
};

} // namespace utility
