set(HASHTABLE_SRC
    insert_command.cpp
    erase_command.cpp
    update_command.cpp
    locked_hashtable.cpp
    directory_entry.cpp
    transaction.cpp
    page.cpp
)

add_library(hashtable STATIC ${HASHTABLE_SRC})