add_library(Server
        Server.cpp
        Server.h
)

target_include_directories(Server PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Server PUBLIC
        Storage
        Codec
        Command
)
