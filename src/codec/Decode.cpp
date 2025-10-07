#include "Decode.h"

namespace codec {
std::string readLine(const std::string& data, size_t& pos)
{
    auto end = data.find(DELIMITER, pos);
    if (end == std::string::npos) {
        throw std::runtime_error("Missing DELIMITER");
    }
    std::string line = data.substr(pos, end - pos);
    pos = end + DELIMITER.size();
    return line;
}

SimpleString DecodeHelpers::readSimpleString(const std::string& data, size_t& pos)
{
    return SimpleString { readLine(data, pos) };
}

Error DecodeHelpers::readError(const std::string& data, size_t& pos)
{
    return Error { readLine(data, pos) };
}

Integer DecodeHelpers::readInteger(const std::string& data, size_t& pos)
{
    try {
        return Integer { std::stoll(readLine(data, pos)) };
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid integer format");
    }
}

BulkString DecodeHelpers::readBulkString(const std::string& data, size_t& pos)
{
    int len;
    try {
        len = std::stoi(readLine(data, pos));
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid BulkString format");
    }

    if (len == -1)
        return BulkString { std::nullopt };

    if (pos + len + DELIMITER.size() > data.size()) {
        throw std::runtime_error("Truncated bulk string");
    }

    std::string bulk = data.substr(pos, len);
    pos += len + DELIMITER.size();
    return BulkString { bulk };
}

Array DecodeHelpers::readArray(const std::string& data, size_t& pos)
{
    int count;
    try {
        count = std::stoi(readLine(data, pos));
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid integer format");
    }

    if (count == -1)
        return Array { {} };

    Array arr;
    arr.elements.reserve(count);
    for (int i = 0; i < count; ++i) {
        arr.elements.push_back(decodeValue(data, pos));
    }
    return arr;
}

}; // namespace codec
