#pragma once

#include "../hashtable/directory_entry.h"

#include <memory>
#include <mutex>
#include <vector>

namespace hashtable {

//==========================================================
// HashtableState declaration
//==========================================================
struct HashtableState
{
    std::vector<DirectoryEntry> m_pDirectory = { 1, DirectoryEntry{ 0 } };
    unsigned int m_globalDepth = { 0 };
};

//==========================================================
// SplitPages declaration
//==========================================================
struct SplitPages
{
    // We will assume that siblingOne has a bit pattern + 0
    // and siblingTwo has a bit pattern + 1
    PagePtr siblingOne;
    PagePtr siblingTwo;
};

//==========================================================
// HashtableMemento declaration
//==========================================================
class HashtableMemento
{
private:
    friend class LockedHashTable;

    explicit HashtableMemento(const HashtableState& state);
    HashtableMemento() = default;

    void SetState(const HashtableState& state) noexcept;
    HashtableState GetState() const noexcept;

private:
    HashtableState m_state = {};
};

//==========================================================
// LockedHashTable declaration
//==========================================================
using MementoPtr = std::unique_ptr<HashtableMemento>;

class LockedHashTable
{
public:
    bool search(int item, SearchResult& output) const noexcept;
    void erase(int item) noexcept;
    void insert(int item) noexcept;
    void update(int item, int newItem) noexcept;

    void SetMemento(MementoPtr memento) noexcept;
    MementoPtr CreateMemento() const noexcept;

private:
    uint32_t getPseudoKey(size_t key, uint32_t depthBits) const noexcept;

    void expandDirectory() noexcept;
    void contractDirectory() noexcept;

    SplitPages splitPage(PagePtr page) const noexcept;
    PagePtr mergePage(PagePtr page1, PagePtr page2) const noexcept;

private:
    HashtableState m_state = {};
    mutable std::mutex m_ceMutex;
};
}  // namespace hashtable
