#pragma once

#include "../hashtable/lockfree_hashtable.h"

#include <atomic>
#include <algorithm>
#include <bitset>
#include <functional>
#include <limits>

//==========================================================
// ContiguousAligned definitions
//==========================================================
template<typename Key, typename Value>
hashtable::ContiguousAligned<Key, Value>::ContiguousAligned(
    Node<Key, Value>* n, bool m)
    : node(n), mark(m) {}

template<typename Key, typename Value>
bool hashtable::ContiguousAligned<Key, Value>::operator!=(
    const ContiguousAligned other)
{
    return !(mark == other.mark && node == other.node);
}

//==========================================================
// ListNode definitions
//==========================================================
template<typename Key, typename Value>
hashtable::ListNode<Key, Value>::ListNode(Node<Key, Value>* node, bool mark)
    : atomicData(new ContiguousAligned<Key, Value>{ node, mark }) {}

template<typename Key, typename Value>
hashtable::ListNode<Key, Value>::ListNode()
    : atomicData(new ContiguousAligned<Key, Value>{}) {}

template<typename Key, typename Value>
bool hashtable::ListNode<Key, Value>::operator!=(const ListNode other)
{
    return !(atomicData == other.atomicData);
}

//==========================================================
// Node definitions
//==========================================================
template<typename Key, typename Value>
hashtable::Node<Key, Value>::Node(DictionaryElement<Key, Value> de, size_t k)
    : item(de), atomicData(new ContiguousAligned<Key, Value>{}), key(k) {}

template<typename Key, typename Value>
hashtable::Node<Key, Value>::Node(size_t key)
    : atomicData(new ContiguousAligned<Key, Value>{}), key(key) {}

//==========================================================
// LockFreeHashtable definitions
//==========================================================
template<typename Key, typename Value>
thread_local std::atomic<hashtable::ListNode<Key, Value>*>
    hashtable::LockFreeHashtable<Key, Value>::next{
        new ListNode<Key, Value>{} };

template<typename Key, typename Value>
thread_local std::atomic<hashtable::ListNode<Key, Value>*>
    hashtable::LockFreeHashtable<Key, Value>::current{
        new ListNode<Key, Value>{} };

template<typename Key, typename Value>
thread_local std::atomic<hashtable::ListNode<Key, Value>*>
    hashtable::LockFreeHashtable<Key, Value>::previous{
        new ListNode<Key, Value>{} };

template<typename Key, typename Value>
hashtable::LockFreeHashtable<Key, Value>::LockFreeHashtable()
    : m_pSegmentTable(new std::atomic<Segment*>[m_size])
{
    std::fill(m_pSegmentTable, m_pSegmentTable + m_size, nullptr);
    setBucket(0, new ListNode<Key, Value>{});
}

template<typename Key, typename Value>
bool hashtable::LockFreeHashtable<Key, Value>::insert(
    DictionaryElement<Key, Value> item) noexcept
{
    auto node = new Node<Key, Value>{ item, regularKey(item.key) };
    auto bucket = node->key % m_size;

    if (!getBucket(bucket))
        initializeBucket(bucket);

    if (!listInsert(getBucket(bucket), node))
    {
        delete node;
        return false;
    }

    auto csize = m_size.load(std::memory_order_acquire);

    if (m_count.fetch_add(1) / csize > kMaxLoad)
        m_size.compare_exchange_strong(csize, csize * 2);

    return true;
}

template<typename Key, typename Value>
bool hashtable::LockFreeHashtable<Key, Value>::find(
    DictionaryElement<Key, Value> item) noexcept
{
    auto key = regularKey(item.key);
    auto bucket = key % m_size.load(std::memory_order_acquire);

    if (!getBucket(bucket))
        initializeBucket(bucket);

    return listFind(getBucket(bucket), key);
}

template<typename Key, typename Value>
bool hashtable::LockFreeHashtable<Key, Value>::remove(
    DictionaryElement<Key, Value> item) noexcept
{
    auto key = regularKey(item.key);
    auto bucket = key % m_size.load(std::memory_order_acquire);

    if (!getBucket(bucket))
        initializeBucket(bucket);

    if (!listDelete(getBucket(bucket), key))
        return false;

    m_count.fetch_sub(1);

    return true;
}

template<typename Key, typename Value>
void hashtable::LockFreeHashtable<Key, Value>::initializeBucket(size_t bucket) noexcept
{
    auto parent = getParent(bucket);

    if (!getBucket(parent))
        initializeBucket(parent);

    auto dummy = new Node<Key, Value>{ dummyKey(bucket) };
    auto dummyPtr = new ListNode<Key, Value>{ dummy, false };

    if (!listInsert(getBucket(parent), dummy))
    {
        delete dummy;
        dummy = current.load()->atomicData.load()->node;
    }

    setBucket(bucket, dummyPtr);
}

template<typename Key, typename Value>
size_t hashtable::LockFreeHashtable<Key, Value>::getParent(size_t bucket) const noexcept
{
    std::bitset<sizeof(size_t) * 8> bits{ bucket };

    for (auto i = bits.size() - 1; i >= 0; --i)
    {
        if (bits[i]) {
            bits.flip(i);
            break;
        }
    }

    return bits.to_ullong();
}

template<typename Key, typename Value>
size_t hashtable::LockFreeHashtable<Key, Value>::reverseKey(size_t key) const noexcept
{
    auto keyBits = static_cast<size_t>(key);
    const auto kSize = sizeof(size_t) * 8;

    std::bitset<kSize> bits{ keyBits };

    for (auto i = 0; i < kSize / 2; ++i) {
        bool temp = bits[i];
        bits.set(i, bits[kSize - i - 1]);
        bits.set(kSize - i - 1, temp);
    }

    return bits.to_ullong();
}

template<typename Key, typename Value>
size_t hashtable::LockFreeHashtable<Key, Value>::regularKey(Key key) const noexcept
{
    return reverseKey(std::hash<Key>{}(key) | std::numeric_limits<long long>::min());
}

template<typename Key, typename Value>
size_t hashtable::LockFreeHashtable<Key, Value>::dummyKey(Key key) const noexcept
{
    return reverseKey(std::hash<Key>{}(key));
}

template<typename Key, typename Value>
hashtable::ListNode<Key, Value>* hashtable::LockFreeHashtable<Key, Value>::getBucket(
    size_t bucket) const noexcept
{
    auto segment = bucket / kSegementSize;

    if (!m_pSegmentTable[segment].load(std::memory_order_acquire))
        return nullptr;

    return m_pSegmentTable[segment].load(std::memory_order_acquire)
        ->table[bucket % kSegementSize].load(std::memory_order_acquire);
}

template<typename Key, typename Value>
void hashtable::LockFreeHashtable<Key, Value>::setBucket(
    size_t bucket, ListNode<Key, Value>* head) noexcept
{
    auto segment = bucket / kSegementSize;
    Segment* newSegment = nullptr;
    Segment* nullSeg = nullptr;

    if (!m_pSegmentTable[segment].load(std::memory_order_acquire))
    {
        newSegment = new Segment{};
        std::fill(newSegment->table, newSegment->table + kSegementSize, nullptr);
    }

    if (!m_pSegmentTable[segment].compare_exchange_strong(nullSeg, newSegment))
        delete newSegment;

    m_pSegmentTable[segment]
        .load(std::memory_order_acquire)
        ->table[bucket % kSegementSize]
        .store(head, std::memory_order_release);
}

//==========================================================
// List definitions
//==========================================================
template<typename Key, typename Value>
bool hashtable::LockFreeHashtable<Key, Value>::listInsert(
    ListNode<Key, Value>* head, Node<Key, Value>* node) noexcept
{
    auto key = node->key;
    auto nodePtr = new ListNode<Key, Value>{ node, false };

    while (true)
    {
        auto currentPtr = current.load(std::memory_order_acquire);
        currentPtr->atomicData.load()->mark = false;

        if (listFind(head, key))
            return false;

        node->atomicData.store(current.load()->atomicData);

        if (previous.compare_exchange_strong(currentPtr, nodePtr))
            return true;
    }

    return false;
}

template<typename Key, typename Value>
bool hashtable::LockFreeHashtable<Key, Value>::listDelete(
    ListNode<Key, Value>* head, size_t key) noexcept
{
    while (true)
    {
        auto nextPtr = new ListNode<Key, Value>{
            next.load().atomicData.load()->node, false };

        auto deletedPtr = new ListNode<Key, Value>{
            next.load().atomicData.load()->node, true };

        auto currentPtr = new ListNode<Key, Value>{
            current.load().atomicData.load()->node, false };

        if (!listFind(head, key))
            return false;

        // This sets the delete "mark" bit of the node
        if (!current.load()->atomicData.load()->node->atomicData.
            compare_exchange_strong(nextPtr->atomicData.load(),
                deletedPtr->atomicData.load()))
            continue;

        if (previous.compare_exchange_strong(currentPtr, nextPtr))
            delete current.load().atomicData.load()->node;
        else
            listFind(head, key);
    }

    return false;
}

template<typename Key, typename Value>
bool hashtable::LockFreeHashtable<Key, Value>::listFind(
    ListNode<Key, Value>* head, size_t key) noexcept
{
try_again:

    previous = head;
    current.store(previous);

    while (true)
    {
        auto currentPtr = current.load();
        currentPtr->atomicData.load()->mark = false;
            
        auto nextPtr = next.load();
        nextPtr->atomicData.load()->mark = false;          

        if (!(current.load(std::memory_order_acquire)->atomicData.load()->node))
            return false;

        next.load()->atomicData.store(current.load()->atomicData.load()->node->atomicData);

        size_t cKey = current.load()->atomicData.load()->node->key;

        if (previous.load(std::memory_order_acquire) != currentPtr)
            goto try_again;
   
        if (!next.load()->atomicData.load()->mark)
        {
            if (cKey >= key)
                return cKey == key;

            previous.load()->atomicData.store(
                current.load()->atomicData.load()->node->atomicData);
        }
        else
        {
            if (previous.compare_exchange_strong(currentPtr, nextPtr))
                delete current.load(std::memory_order_release)->atomicData.load()->node;
            else
                goto try_again;

        }

        current.store(next, std::memory_order_release);
    }

    return false;
}
