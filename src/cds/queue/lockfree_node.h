#pragma once

#include <atomic>

namespace queue { namespace lf {
template<typename T>
struct Node;

template<typename T>
struct alignas(2 * sizeof(void*)) NodePtr
{
    Node<T>* ptr;
    size_t count;

    NodePtr() = default;
    NodePtr(Node<T>* p, size_t c)
        : ptr(std::move(p)), count(c) {}

    bool operator==(const NodePtr& other) {
        return other.ptr == ptr && other.count == count;
    }
};

template<typename T>
struct Node
{
    T value;
    std::atomic<NodePtr<T>> next;
};

}  // namespace lf
} // namespace stack
