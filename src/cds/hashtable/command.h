#pragma once

#include "../hashtable/locked_hashtable.h"

namespace hashtable {

class Command
{
public:
    virtual ~Command() {}

    virtual void Execute() = 0;
    virtual void Undo() = 0;

protected:
    Command() = default;
    explicit Command(std::shared_ptr<LockedHashTable> receiver)
        : m_receiver{ receiver } {}

protected:
    std::shared_ptr<LockedHashTable> m_receiver = { nullptr };
    std::unique_ptr<HashtableMemento> m_memento = { nullptr };
};
}  // namespace hashtable