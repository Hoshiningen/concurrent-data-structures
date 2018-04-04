#pragma once

#include <atomic>
#include <vector>
#include <utility>

namespace hashtable {

template<typename Key, typename Value>
struct DictionaryElement
{
    DictionaryElement() = default;

    Key key;
    Value value;
};

template<typename Key, typename Value>
struct Node;

template<typename Key, typename Value>
struct ContiguousAligned
{
    ContiguousAligned(Node<Key, Value>*, bool);
    ContiguousAligned() = default;

    bool operator!=(const ContiguousAligned other);

    Node<Key, Value>* node;
    bool mark;
};

template<typename Key, typename Value>
struct ListNode
{
    ListNode(Node<Key, Value>*, bool);
    ListNode();

    bool operator!=(const ListNode other);

    std::atomic<ContiguousAligned<Key, Value>*> atomicData = {};
};

template<typename Key, typename Value>
struct Node
{
    Node(DictionaryElement<Key, Value>, size_t);
    explicit Node(size_t key);

    DictionaryElement<Key, Value> item = {};
    std::atomic<ContiguousAligned<Key, Value>*> atomicData = {};
    size_t key = 0;
};

template<typename Key, typename Value>
class LockFreeHashtable
{
public:
    LockFreeHashtable();

    LockFreeHashtable(const LockFreeHashtable&) = delete;
    LockFreeHashtable& operator=(const LockFreeHashtable&) = delete;

    bool insert(DictionaryElement<Key, Value> item) noexcept;
    bool find(DictionaryElement<Key, Value> item) noexcept;
    bool remove(DictionaryElement<Key, Value> item) noexcept;

private:
    void initializeBucket(size_t bucket) noexcept;
    size_t getParent(size_t bucket) const noexcept;

    size_t reverseKey(size_t key) const noexcept;
    size_t regularKey(Key key) const noexcept;
    size_t dummyKey(Key key) const noexcept;
    
    bool listInsert(ListNode<Key, Value>* head, Node<Key, Value>* node) noexcept;
    bool listDelete(ListNode<Key, Value>* head, size_t key) noexcept;
    bool listFind(ListNode<Key, Value>* head, size_t key)  noexcept;

    ListNode<Key, Value>* getBucket(size_t bucket) const noexcept;
    void setBucket(size_t bucket, ListNode<Key, Value>* head) noexcept;

private:
    static const int kMaxLoad = 1000;
    static const int kSegementSize = 1000;

    struct Segment
    {
        std::atomic<ListNode<Key, Value>*> table[kSegementSize];
    };

    std::atomic<uint32_t> m_count = { 0 };
    std::atomic<uint32_t> m_size = { 2 };
    std::atomic<Segment*>* m_pSegmentTable = { nullptr };

    static thread_local std::atomic<ListNode<Key, Value>*> next;
    static thread_local std::atomic<ListNode<Key, Value>*> current;
    static thread_local std::atomic<ListNode<Key, Value>*> previous;
};

}  // namespace hashtable

#include "../hashtable/lockfree_hashtable_impl.h"