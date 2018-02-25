#include "../hashtable/insert_command.h"
#include "../hashtable/locked_hashtable.h"

#include <memory>

hashtable::InsertCommand::InsertCommand(
    std::shared_ptr<hashtable::LockedHashTable> receiver, int item)
    : Command{ receiver }, m_item{ item } {}

void hashtable::InsertCommand::Execute()
{
    m_memento = m_receiver->CreateMemento();
    m_receiver->insert(m_item);
}

void hashtable::InsertCommand::Undo()
{
    m_receiver->SetMemento(std::move(m_memento));
}