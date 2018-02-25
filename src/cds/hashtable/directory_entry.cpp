#include "../hashtable/directory_entry.h"

hashtable::DirectoryEntry::DirectoryEntry(unsigned int vb)
    : m_vb{ vb } {}

const unsigned int hashtable::DirectoryEntry::getVerificationBits() const noexcept
{
    return m_vb;
}

void hashtable::DirectoryEntry::setVerificationBits(unsigned int vb) noexcept
{
    m_vb = vb;
}

const unsigned int hashtable::DirectoryEntry::getPageStatus() const noexcept
{
    return m_pPage->getPageStatus();
}

const unsigned int hashtable::DirectoryEntry::getPageLocalDepth() const noexcept
{
    return m_pPage->getLocalDepth();
}

std::shared_ptr<hashtable::Page> hashtable::DirectoryEntry::getPage() const noexcept
{
    return m_pPage;
}