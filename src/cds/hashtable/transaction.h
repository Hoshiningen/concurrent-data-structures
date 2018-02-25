#pragma once

#include "../hashtable/command.h"
#include "../../utility/memory.h"

#include <memory>
#include <vector>

namespace hashtable {

class DBTransaction
{
public:

    void AddCommand(std::unique_ptr<Command> command) noexcept;

    void Commit() noexcept;
    void Rollback() noexcept;

private:
    std::vector<std::unique_ptr<Command>> m_commands =
        std::vector<std::unique_ptr<Command>>{ };
};
}  //namespace hashtable