add_executable(Server_test
        Server_test.cpp
)

target_link_libraries(Server_test
        PRIVATE
        Server
        Command
        Codec
        Storage
        gtest_main
        gmock_main
)

target_include_directories(Server_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)
