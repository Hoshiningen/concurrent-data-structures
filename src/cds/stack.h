#pragma once

#include <memory>

namespace stack {

////////////////////////////////////////////////////////////
//
// Represents a node used in linked list implementations
//
////////////////////////////////////////////////////////////
class node {
public:
    node()
        : m_value(0), m_pNext(nullptr) {}

    node(int value)
        : m_value(value), m_pNext(nullptr) {}

    ~node() { 
        delete m_pNext;
        m_pNext = nullptr;
    }

    void set_value(int value) { m_value = value; }
    void set_next(node* next) { m_pNext = next; }
    
    int get_value() { return m_value; }
    node* get_next() { return m_pNext; }

 private:
    int m_value;
    node* m_pNext;
};

////////////////////////////////////////////////////////////
//
// Represents an abstract base class for all stack
// implementations
//
////////////////////////////////////////////////////////////
class stack_base {
public:
    virtual ~stack_base() = 0 {}

    virtual void push(int value) = 0;
    virtual bool pop(int& out) = 0;
};
}  // namespace stack
