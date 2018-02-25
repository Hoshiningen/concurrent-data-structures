#pragma once

#include "../../cds/queue/locked_queue.h"
#include "../../cds/hashtable/transaction.h"

#include <memory>

namespace hashtable {

class DatabaseEngine
{
public:
    DatabaseEngine(const DatabaseEngine& other) = delete;
    DatabaseEngine& operator=(const DatabaseEngine& other) = delete;

    static DatabaseEngine& instance();
    
    void AddTransaction(DBTransaction transaction) noexcept;

private:
    DatabaseEngine() = default;

private:
    queue::LockedQueue<DBTransaction> m_IOQueue = { };
    queue::LockedQueue<DBTransaction> m_CP1Queue = { };
    queue::LockedQueue<DBTransaction> m_CP2Queue = { };
    queue::LockedQueue<DBTransaction> m_DelayQueue = { };
    queue::LockedQueue<DBTransaction> m_PendingQueue = { };
    queue::LockedQueue<DBTransaction> m_BlockingQueue = { };
};
}  // hashtable
