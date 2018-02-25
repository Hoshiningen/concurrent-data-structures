#include "../hashtable/transaction.h"
#include <algorithm>

void hashtable::DBTransaction::AddCommand(std::unique_ptr<Command> command) noexcept
{
    m_commands.push_back(std::move(command));
}

void hashtable::DBTransaction::Commit() noexcept
{
    std::for_each(m_commands.begin(), m_commands.end(),
        [](const std::unique_ptr<Command>& command) { command->Execute(); });
}

void hashtable::DBTransaction::Rollback() noexcept
{
    std::for_each(m_commands.rbegin(), m_commands.rend(),
        [](const std::unique_ptr<Command>& command) { command->Undo(); });
}