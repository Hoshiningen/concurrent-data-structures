#include "../hashtable/locked_hashtable.h"

#include "../hashtable/directory_entry.h"
#include "../hashtable/Page.h"
#include "../hashtable/database_engine.h"

#include "../../utility/memory.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

//==========================================================
// This makes constructing a memento a one-liner, which is nice
//
// \param - state   The state to save in the memento
//==========================================================
hashtable::HashtableMemento::HashtableMemento(const HashtableState& state)
    : m_state{ state } {}

//==========================================================
// This sets the state contained in the memento
//
// \param - state   The state to save in the memento
//==========================================================
void hashtable::HashtableMemento::SetState(const hashtable::HashtableState& state) noexcept
{
    m_state = state;
}

//==========================================================
// This gets the state contained in the memento
//
// \return -        The memento state
//==========================================================
hashtable::HashtableState hashtable::HashtableMemento::GetState() const noexcept
{
    return m_state;
}

//==========================================================
// LockedHashTable definitions
//==========================================================

//==========================================================
// This performs a search on the hashtable, where the success
// of the search is returned as a boolean, and also the result
// of the search (the correct page and local depth), will be
// passed back to the calling method.
//
// \param - item    The value to search for
// \param - output  The results of the search, which is the 
//                  page and local depth of a search
//
// \return -        The success of finding the item in the
//                  hash table
//==========================================================
bool hashtable::LockedHashTable::search(int item, SearchResult& output) const noexcept
{
    auto key = std::hash<int>{}(item);
    auto gBits = 0U;
    auto dirIndex = 0U;
    auto foundDir = m_state.m_pDirectory.end();

    // Lets us return the search results outside the function
    auto localDepth = 0;
    std::shared_ptr<Page> page = nullptr;
    
    do
    {
        gBits = static_cast<unsigned int>
            (std::log2(static_cast<double>(m_state.m_pDirectory.size())));

        dirIndex = getPseudoKey(key, gBits);

        page = m_state.m_pDirectory[dirIndex].getPage();
        localDepth = m_state.m_pDirectory[dirIndex].getPageLocalDepth();

    } while (dirIndex != m_state.m_pDirectory[dirIndex].getVerificationBits() &&
             (m_state.m_pDirectory[dirIndex].getVerificationBits() <
              m_state.m_pDirectory[dirIndex].getPageLocalDepth()));

    output.page = page;
    output.localDepth = localDepth;

    return page->search(item);
}

//==========================================================
// This removes the specified value from the hash table
//
// \param - value   The value to insert into the hash table
//==========================================================
void hashtable::LockedHashTable::erase(int value) noexcept
{
    SearchResult output{};
    bool searchValid = search(value, output);
    bool correctPage = false;

    do
    {
        if (searchValid = search(value, output))
        {
            // Lock the page and compare..
            correctPage = output.localDepth == output.page->getLocalDepth();
        }

    } while (searchValid && correctPage);

    
}

//==========================================================
// This inserts the specified value into the hash table
//
// \param - value   The value to insert into the hash table
//==========================================================
void hashtable::LockedHashTable::insert(int value) noexcept
{
    SearchResult output{};
    bool searchValid = search(value, output);
    bool correctPage = false;

    do
    {
        searchValid = search(value, output);

        // Lock the page and compare..
        correctPage = output.localDepth == output.page->getLocalDepth();

    } while (!correctPage);

    // if page is full, prepare for a page split
    if (output.page->isFull())
    {
        // prepare for a page split
        if (output.localDepth == m_state.m_globalDepth)
        {
            // A directory expansion is triggered from the split
        }
    }
    else
    {
        // insert normally...
    }
}

//==========================================================
// This updates the specified value with the new value
//
// \param - value       The value to update
// \param - newValue    The value that will replace the old
//                      value
//==========================================================
void hashtable::LockedHashTable::update(int value, int newValue) noexcept
{
    SearchResult output{};
    bool searchValid = search(value, output);
    bool correctPage = false;

    do
    {
        searchValid = search(value, output);

        // Lock the page and compare..
        correctPage = output.localDepth == output.page->getLocalDepth();

    } while (searchValid && correctPage);

    if (searchValid)
        output.page->update(value, newValue);
}

//==========================================================
// This creates a new memento that stores the current state
// of the hashtable
//
// \return - A unique ptr to memento
//==========================================================
hashtable::MementoPtr hashtable::LockedHashTable::CreateMemento() const noexcept
{
    // Can't use make_unique because of the narrow interface in the memento pattern
    return std::unique_ptr<HashtableMemento>(new HashtableMemento{ m_state });
}

//==========================================================
// This loads the memento's state and updates the current 
// hashtable state with it
//
// \param - memento     The memento to load the state from
//==========================================================
 void hashtable::LockedHashTable::SetMemento(hashtable::MementoPtr memento) noexcept
{
     if (memento)
         m_state = memento->GetState();
}

 //==========================================================
 // This extracts the the value stored in the first [depthBits]
 // bits of the key
 //
 // \param - key        The original hash key
 // \param - depthBits  The number of bits of the pseudokey
 //                     to extract
 //
 // \return - The extracted pseudokey
 //==========================================================
 uint32_t hashtable::LockedHashTable::getPseudoKey(size_t key, uint32_t depthBits) const noexcept
 {
     const auto numBits = sizeof(key) * 8;

     uint64_t mask = !depthBits ? 0 :
         static_cast<size_t>(-1) >> (numBits - depthBits);

     return mask & key;
 }

 //==========================================================
 // This performs a page split, which will create two separate
 // pages that the items from the given page are redistributed
 // into.
 //
 // \param - page       The page to split
 //
 // \return -   The sibling pages that result from splitting the
 //             the parent
 //==========================================================
hashtable::SplitPages hashtable::LockedHashTable::splitPage(PagePtr page) const noexcept
{
    const uint32_t localDepth = page->getLocalDepth();
    const uint32_t siblingDepth = localDepth << 1;

    SplitPages output;
    output.siblingOne = std::make_shared<Page>();
    output.siblingOne->setLocalDepth(localDepth + 1);

    output.siblingTwo = std::make_shared<Page>();
    output.siblingTwo->setLocalDepth(localDepth + 1);

    for (const auto& item : *page)
    {
        uint32_t pseudoKey = getPseudoKey(std::hash<int>{}(item), localDepth + 1);

        // The pages will need to be locked before inserting the items
        if (pseudoKey == siblingDepth)
            output.siblingOne->insert(item);
        else if (pseudoKey == siblingDepth + 1)
            output.siblingTwo->insert(item);
    }
    
    return output;
}

//==========================================================
// This performs a page merge, which merges two pages together
// and returns a single page containing all elements from both
//
// \param - page1   The first sibling to merge
// \param - page2   The second sibling to merge
//
// \return -    The merged page that contains all items from
//              the two sibling pages
//==========================================================
hashtable::PagePtr hashtable::LockedHashTable::mergePage(
    PagePtr page1, PagePtr page2) const noexcept
{
    auto mergedPage = std::make_shared<Page>();

    // The pages may need a read lock?
    for (const auto& item : *page1)
        mergedPage->insert(item);

    for (const auto& item : *page2)
        mergedPage->insert(item);

    mergedPage->setLocalDepth(std::log2(mergedPage->getPageStatus()));

    return mergedPage;
}

void hashtable::LockedHashTable::expandDirectory() noexcept
{
    std::lock_guard<std::mutex> ceLock{ m_ceMutex };
    // All pages of the previous directory need to index
    // into the same cells, and all new ones from the expansion
    // need to point to the same page
}

void hashtable::LockedHashTable::contractDirectory() noexcept
{
    std::lock_guard<std::mutex> ceLock{ m_ceMutex };
}