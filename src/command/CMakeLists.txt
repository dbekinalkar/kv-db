add_library(Command
    CommandProcessor.cpp
    CommandProcessor.h
    StringCommands.cpp
    StringCommands.h
    CommandHelpers.cpp
    CommandHelpers.h
    ListCommands.cpp
    ListCommands.h
    SetCommands.cpp
    SetCommands.h
    HashCommands.cpp
    HashCommands.h
    SortedSetCommands.cpp
    SortedSetCommands.h
)

target_include_directories(Command PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Command PUBLIC
    Storage
    Codec
)
