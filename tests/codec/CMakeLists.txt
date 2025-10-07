add_executable(Codec_test
        Codec_test.cpp
)

target_link_libraries(Codec_test
        PRIVATE
        Codec
        gtest_main
        gmock_main
)

target_include_directories(Codec_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)
