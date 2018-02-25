#pragma once

#include <memory>
#include <unordered_set>
#include <shared_mutex>

//==========================================================
// Page declaration
//==========================================================
namespace hashtable {
class Page;
using PagePtr = std::shared_ptr<Page>;

class Page
{
public:
    using Iterator = std::unordered_set<int>::iterator;

    const unsigned int getLocalDepth() const noexcept;
    void setLocalDepth(unsigned int depth) noexcept;

    const unsigned int getPageStatus() const noexcept;

    bool search(int item) const noexcept;
    void erase(int item) noexcept;
    void insert(int item) noexcept;
    void update(int item, int newItem) noexcept;

    const bool isFull() const noexcept;

    Iterator begin() const noexcept;
    Iterator end() const noexcept;

private:
    mutable std::shared_mutex m_rwMutex;
    std::unordered_set<int> m_hashSet = {};
    unsigned int m_localDepth = { 0 };
};
}  // namespace hashtable