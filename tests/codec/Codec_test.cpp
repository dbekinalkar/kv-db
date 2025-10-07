#include "Codec.h"
#include "CodecValue.h"
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace codec;

void TestRoundTrip(const CodecValue& cv_in, const std::string& expected_resp)
{
    // 1. Encode
    std::string encoded_data = Codec::encode(cv_in);

    // 2. Optional: Check if the encoded data matches the expected RESP string
    // NOTE: This is a stronger test than just checking the prefix.
    EXPECT_EQ(encoded_data, expected_resp)
        << "Encoded RESP does not match expected string."
        << "\nExpected: " << expected_resp
        << "\nActual:   " << encoded_data;

    // 3. Decode
    CodecValue cv_out = Codec::decode(encoded_data);

    // 4. Check for equality (using the custom comparator)
    ASSERT_EQ(cv_in, cv_out)
        << "Decoded value does not match original value after round-trip.";
}

// --- GTest Test Cases ---

// Simple String (RESP: +<value>\r\n)
TEST(CodecTest, SimpleString)
{
    CodecValue cv_in = ok(); // +OK
    std::string expected_resp = "+OK\r\n";
    TestRoundTrip(cv_in, expected_resp);

    cv_in = CodecValue { SimpleString { "PONG" } };
    expected_resp = "+PONG\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Error (RESP: -<value>\r\n)
TEST(CodecTest, Error)
{
    CodecValue cv_in = err("ERR unknown command 'FOO'");
    std::string expected_resp = "-ERR unknown command 'FOO'\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Integer (RESP: :<value>\r\n)
TEST(CodecTest, Integer)
{
    // Positive
    CodecValue cv_in = integer(12345LL);
    std::string expected_resp = ":12345\r\n";
    TestRoundTrip(cv_in, expected_resp);

    // Zero
    cv_in = integer(0LL);
    expected_resp = ":0\r\n";
    TestRoundTrip(cv_in, expected_resp);

    // Negative
    cv_in = integer(-100LL);
    expected_resp = ":-100\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Bulk String (RESP: $<length>\r\n<data>\r\n)
TEST(CodecTest, BulkString)
{
    // Standard string
    CodecValue cv_in = bulk("hello world"); // $11\r\nhello world\r\n
    std::string expected_resp = "$11\r\nhello world\r\n";
    TestRoundTrip(cv_in, expected_resp);

    // Empty string
    cv_in = bulk(""); // $0\r\n\r\n
    expected_resp = "$0\r\n\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Null Bulk String (RESP: $-1\r\n)
TEST(CodecTest, NullBulkString)
{
    CodecValue cv_in = nullBulk(); // $-1\r\n
    std::string expected_resp = "$-1\r\n";

    // Test the encode path
    std::string encoded_data = Codec::encode(cv_in);
    EXPECT_EQ(encoded_data, expected_resp);

    // Test the decode path
    CodecValue cv_out = Codec::decode(expected_resp);

    // Check if the decoded value is a BulkString with a nullopt value
    ASSERT_TRUE(std::holds_alternative<BulkString>(cv_out.data));
    EXPECT_FALSE(std::get<BulkString>(cv_out.data).value.has_value());

    // Test round-trip using the custom comparator (should also pass)
    ASSERT_EQ(cv_in, cv_out)
        << "Decoded nullBulk() does not match original value.";
}

// Array (RESP: *<size>\r\n<element1>...<elementN>)
TEST(CodecTest, Array_Basic)
{
    std::vector<CodecValue> elements;
    elements.push_back(integer(1LL));
    elements.push_back(bulk("foo"));
    elements.push_back(ok());

    CodecValue cv_in = array(elements);
    // *3\r\n:1\r\n$3\r\nfoo\r\n+OK\r\n
    std::string expected_resp = "*3\r\n:1\r\n$3\r\nfoo\r\n+OK\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Array with Empty Elements
TEST(CodecTest, Array_WithEmptyElements)
{
    std::vector<CodecValue> elements;
    elements.push_back(bulk("")); // Empty Bulk String ($0\r\n\r\n)
    elements.push_back(array({})); // Empty Array (*0\r\n)
    elements.push_back(nullBulk()); // Null Bulk String ($-1\r\n)

    CodecValue cv_in = array(elements);
    // *3\r\n$0\r\n\r\n*0\r\n$-1\r\n
    std::string expected_resp = "*3\r\n$0\r\n\r\n*0\r\n$-1\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Array with Nested Array
TEST(CodecTest, Array_Nested)
{
    std::vector<CodecValue> inner_elements;
    inner_elements.push_back(integer(10LL));
    inner_elements.push_back(bulk("bar"));

    std::vector<CodecValue> outer_elements;
    outer_elements.push_back(bulk("foo"));
    outer_elements.push_back(array(inner_elements)); // Nested Array

    CodecValue cv_in = array(outer_elements);
    // *2\r\n$3\r\nfoo\r\n*2\r\n:10\r\n$3\r\nbar\r\n
    std::string expected_resp = "*2\r\n$3\r\nfoo\r\n*2\r\n:10\r\n$3\r\nbar\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Empty Array (RESP: *0\r\n)
TEST(CodecTest, Array_Empty)
{
    CodecValue cv_in = array({}); // *0\r\n
    std::string expected_resp = "*0\r\n";
    TestRoundTrip(cv_in, expected_resp);
}

// Invalid RESP Input (Decode failure tests)
TEST(CodecTest, InvalidInput_Decode)
{
    // 1. Truncated bulk string data
    EXPECT_THROW(Codec::decode("$5\r\nhello"), std::runtime_error);

    // 2. Invalid type marker
    EXPECT_THROW(Codec::decode("!4\r\nabcd\r\n"), std::runtime_error);

    // 3. Bulk string length not a number
    EXPECT_THROW(Codec::decode("$abc\r\n"), std::runtime_error);

    // 4. Missing final CRLF on Simple String
    EXPECT_THROW(Codec::decode("+OK"), std::runtime_error);

    // 5. Array size not a number
    EXPECT_THROW(Codec::decode("*X\r\n"), std::runtime_error);
}

// Empty Input
TEST(CodecTest, EmptyInput_Decode)
{
    EXPECT_THROW(Codec::decode(""), std::runtime_error);
}
