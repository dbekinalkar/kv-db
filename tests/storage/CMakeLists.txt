add_executable(KeyValueStore_test
    KeyValueStore_test.cpp
)

target_link_libraries(KeyValueStore_test
    PRIVATE
        Storage
        gtest_main
        gmock_main
)

target_include_directories(KeyValueStore_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)