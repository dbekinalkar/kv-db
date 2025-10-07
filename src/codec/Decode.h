#pragma once
#include "CodecValue.h"
#include "Marker.h"
#include <functional>
#include <map>
#include <stdexcept>

using namespace std::string_literals;

namespace codec {

struct DecodeHelpers {
    static SimpleString readSimpleString(const std::string& d, size_t& p);
    static Error readError(const std::string& d, size_t& p);
    static Integer readInteger(const std::string& d, size_t& p);
    static BulkString readBulkString(const std::string& d, size_t& p);
    static Array readArray(const std::string& d, size_t& p);
};

struct DecodeDispatch {
    using Fn = std::function<CodecValue(const std::string&, size_t&)>;
    static const std::map<char, Fn>& table()
    {
        static const std::map<MarkerType, Fn> t = {
            { MARKER_SIMPLE_STRING, [](auto& d, auto& p) { return CodecValue { DecodeHelpers::readSimpleString(d, p) }; } },
            { MARKER_ERROR, [](auto& d, auto& p) { return CodecValue { DecodeHelpers::readError(d, p) }; } },
            { MARKER_INTEGER, [](auto& d, auto& p) { return CodecValue { DecodeHelpers::readInteger(d, p) }; } },
            { MARKER_BULK_STRING, [](auto& d, auto& p) { return CodecValue { DecodeHelpers::readBulkString(d, p) }; } },
            { MARKER_ARRAY, [](auto& d, auto& p) { return CodecValue { DecodeHelpers::readArray(d, p) }; } }
        };
        return t;
    }
};

inline CodecValue decodeValue(const std::string& data, size_t& pos)
{
    if (pos >= data.size())
        throw std::runtime_error("Unexpected end of input");
    char prefix = data[pos++];
    auto it = DecodeDispatch::table().find(prefix);
    if (it == DecodeDispatch::table().end())
        throw std::runtime_error("Unknown prefix: "s + prefix);
    return it->second(data, pos);
}

} // namespace codec
