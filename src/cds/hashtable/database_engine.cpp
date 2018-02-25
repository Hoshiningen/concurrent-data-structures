#include "../hashtable/database_engine.h"

hashtable::DatabaseEngine& hashtable::DatabaseEngine::instance()
{
    static DatabaseEngine uniqueInstance{};
    return uniqueInstance;
}

void hashtable::DatabaseEngine::AddTransaction(
    DBTransaction transaction) noexcept
{
    m_PendingQueue.enqueue(transaction);
}