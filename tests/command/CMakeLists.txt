add_executable(CommandProcessor_test
        CommandProcessor_test.cpp
)

target_link_libraries(CommandProcessor_test
        PRIVATE
        Command
        Codec
        Storage
        gtest_main
        gmock_main
)

target_include_directories(CommandProcessor_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)
