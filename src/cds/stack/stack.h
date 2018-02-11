#pragma once

#include <memory>

namespace stack {

////////////////////////////////////////////////////////////
//
// Represents an abstract base class for all node
// implementations
//
////////////////////////////////////////////////////////////
class node_base {
public:

    virtual ~node_base() { }

    virtual void set_value(int value) = 0;
    virtual int get_value() = 0;

    virtual void set_next(node_base* next) = 0;
    virtual node_base* get_next() = 0;
};

////////////////////////////////////////////////////////////
//
// Represents a stack node that keeps track of access counts
//
////////////////////////////////////////////////////////////
class access_cnt_node : public node_base {
public:
    access_cnt_node(int value)
        : m_pNext(nullptr), m_value(value), m_count(0) { }

    ~access_cnt_node() {
        delete m_pNext;
        m_pNext = nullptr;
    }

    virtual void set_value(int value) { m_value = value; }
    virtual int get_value() { return m_value; }

    virtual void set_next(node_base* next) { m_pNext = std::move(next); }
    virtual node_base* get_next() { return m_pNext; }

    virtual void increment() { m_count++; }

private:
    node_base* m_pNext;
    int m_value;
    unsigned int m_count;
};

////////////////////////////////////////////////////////////
//
// Represents a standard stack node
//
////////////////////////////////////////////////////////////
class node : public node_base {
public:
    node()
        : m_value(0), m_pNext(nullptr) {}

    node(int value)
        : m_value(value), m_pNext(nullptr) {}

    ~node() { 
        delete m_pNext;
        m_pNext = nullptr;
    }

    virtual void set_value(int value) override { m_value = value; }
    virtual void set_next(node_base* next) override { m_pNext = next; }
    
    virtual int get_value() override { return m_value; }
    virtual node_base* get_next() override { return m_pNext; }

 private:
    int m_value;
    node_base* m_pNext;
};

////////////////////////////////////////////////////////////
//
// Represents an abstract base class for all stack
// implementations
//
////////////////////////////////////////////////////////////
class stack_base {
public:
    virtual ~stack_base() { };

    virtual void push(int value) = 0;
    virtual bool pop(int& out) = 0;
};
}  // namespace stack
