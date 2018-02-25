#include "../hashtable/update_command.h"
#include "../hashtable/locked_hashtable.h"

#include <memory>

hashtable::UpdateCommand::UpdateCommand(
    std::shared_ptr<hashtable::LockedHashTable> receiver, int oldItem, int newItem)
    : Command{ receiver }, m_oldItem{ oldItem }, m_newItem{ newItem } {}

void hashtable::UpdateCommand::Execute()
{
    m_memento = m_receiver->CreateMemento();
    m_receiver->update(m_oldItem, m_newItem);
}

void hashtable::UpdateCommand::Undo()
{
    m_receiver->SetMemento(std::move(m_memento));
}