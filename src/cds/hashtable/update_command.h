#pragma once

#include "../hashtable/command.h"
#include "../hashtable/locked_hashtable.h"

namespace hashtable {

class UpdateCommand : public Command
{
public:
    UpdateCommand(std::shared_ptr<LockedHashTable> receiver, int oldItem, int newItem);

    virtual void Execute() override;
    virtual void Undo() override;

private:
    int m_oldItem = { 0 };
    int m_newItem = { 0 };
};
}  // namespace hashtable
