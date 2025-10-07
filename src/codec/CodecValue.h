#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace codec {

struct SimpleString {
    std::string value;
};
struct Error {
    std::string value;
};
struct Integer {
    long long value;
};
struct BulkString {
    std::optional<std::string> value;
};

struct CodecValue;
struct Array {
    std::vector<CodecValue> elements;
};

using CodecVariant = std::variant<SimpleString, Error, Integer, BulkString, Array>;
struct CodecValue {
    CodecVariant data;
};

inline CodecValue ok()
{
    return CodecValue { SimpleString { "OK" } };
}

inline CodecValue err(const std::string& msg)
{
    return CodecValue { Error { msg } };
}

inline CodecValue integer(long long val)
{
    return CodecValue { Integer { val } };
}

inline CodecValue bulk(const std::string& s)
{
    return CodecValue { BulkString { s } };
}

inline CodecValue nullBulk()
{
    return CodecValue { BulkString { std::nullopt } };
}

inline CodecValue array(const std::vector<CodecValue>& vals)
{
    return CodecValue { Array { vals } };
}

bool operator==(const CodecValue& lhs, const CodecValue& rhs);

} // namespace codec