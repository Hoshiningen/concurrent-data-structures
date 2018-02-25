#include "../hashtable/Page.h"

//==========================================================
// Page Implementation definitions
//==========================================================
const unsigned int hashtable::Page::getLocalDepth() const noexcept
{
    return m_localDepth;
}

void hashtable::Page::setLocalDepth(unsigned int depth) noexcept
{
    m_localDepth = depth;
}

const unsigned int hashtable::Page::getPageStatus() const noexcept
{
    return m_hashSet.size();
}

bool hashtable::Page::search(int item) const noexcept
{
    return m_hashSet.find(item) != m_hashSet.end();
}

void hashtable::Page::erase(int item) noexcept
{
    auto found = m_hashSet.find(item);

    if (found != m_hashSet.end())
        m_hashSet.erase(found);
}

void hashtable::Page::insert(int item) noexcept
{
    auto found = m_hashSet.find(item);

    if (found == m_hashSet.end())
        m_hashSet.insert(item);
}

void hashtable::Page::update(int item, int newItem) noexcept
{
    erase(item);
    insert(newItem);
}

const bool hashtable::Page::isFull() const noexcept
{
    return m_hashSet.size() == std::pow(2, m_localDepth);
}

hashtable::Page::Iterator hashtable::Page::begin() const noexcept
{
    return m_hashSet.cbegin();
}

std::unordered_set<int>::const_iterator hashtable::Page::end() const noexcept
{
    return m_hashSet.cend();
}