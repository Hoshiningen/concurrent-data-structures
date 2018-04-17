#pragma once

namespace stack { namespace lf {
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
};

template<typename T>
struct Node
{
    T value;
    NodePtr<T> next;
};

}  // namespace lf
} // namespace stack
