add_subdirectory(storage)
add_subdirectory(codec)
add_subdirectory(command)
add_subdirectory(server)

add_executable(
    kvdb
    main.cpp
)

target_link_libraries(
    kvdb
    PRIVATE
    Storage
    Codec
    Command
    Server
)