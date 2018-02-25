#pragma once

#include "../hashtable/command.h"
#include "../hashtable/locked_hashtable.h"

namespace hashtable {

class InsertCommand : public Command
{
public:
    InsertCommand(std::shared_ptr<LockedHashTable> receiver, int item);

    virtual void Execute() override;
    virtual void Undo() override;

private:
    int m_item = { 0 };
};
}  // namespace hashtable
