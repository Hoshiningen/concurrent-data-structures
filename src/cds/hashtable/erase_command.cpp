#include "../hashtable/erase_command.h"
#include "../hashtable/locked_hashtable.h"

#include <memory>

hashtable::EraseCommand::EraseCommand(
    std::shared_ptr<hashtable::LockedHashTable> receiver, int item)
    : Command{ receiver }, m_item{ item } {}

void hashtable::EraseCommand::Execute()
{
    m_memento = m_receiver->CreateMemento();
    m_receiver->erase(m_item);
}

void hashtable::EraseCommand::Undo()
{
    m_receiver->SetMemento(std::move(m_memento));
}