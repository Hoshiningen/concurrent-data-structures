#pragma once

#include "../hashtable/Page.h"

#include <memory>

//==========================================================
// Directory declaration
//==========================================================
//using SearchResult = std::tuple<std::shared_ptr<Page>, unsigned int>;

namespace hashtable {

struct SearchResult
{
    PagePtr page = { nullptr };
    unsigned int localDepth = { 0 };
};

class DirectoryEntry
{
public:
    explicit DirectoryEntry(unsigned int vb);

    const unsigned int getVerificationBits() const noexcept;
    void setVerificationBits(unsigned int vb) noexcept;

    const unsigned int getPageStatus() const noexcept;
    const unsigned int getPageLocalDepth() const noexcept;

    PagePtr getPage() const noexcept;

private:
    unsigned int m_vb = { 0 };
    PagePtr m_pPage = std::make_shared<Page>();
};
}  // namespace hashtable

// 0 - d, a

// 00 
// 01

// 00
// 01
// 10
// 11