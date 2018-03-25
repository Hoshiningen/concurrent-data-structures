#pragma once

#include <atomic>

namespace queue { namespace lf {
template<typename T>
struct Node;

// Even with 16 byte aligned structs (of size 16),
// the cmpxch16b instruction doesn't get generated for
// the compare and swap. It behaves correctly, but I 
// think it doesn't perform as well as it should.
template<typename T>
struct alignas(2 * sizeof(void*)) NodePtr
{
    Node<T>* ptr;
    size_t count;

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
