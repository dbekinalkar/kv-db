#pragma once
#include "CodecValue.h"
#include "Marker.h"
#include <string>

using namespace std::literals::string_literals;

namespace codec {

struct EncoderVisitor {
    std::string operator()(const SimpleString& s) const
    {
        return MARKER_SIMPLE_STRING + s.value + DELIMITER;
    }
    std::string operator()(const Error& e) const
    {
        return MARKER_ERROR + e.value + DELIMITER;
    }
    std::string operator()(const Integer& i) const
    {
        return MARKER_INTEGER + std::to_string(i.value) + DELIMITER;
    }
    std::string operator()(const BulkString& b) const
    {
        if (!b.value)
            return MARKER_BULK_STRING + "-1"s + DELIMITER;
        return MARKER_BULK_STRING + std::to_string(b.value->size()) + DELIMITER + *b.value + DELIMITER;
    }
    std::string operator()(const Array& arr) const
    {
        std::string out = MARKER_ARRAY + std::to_string(arr.elements.size()) + DELIMITER;
        for (const auto& [data] : arr.elements) {
            out += std::visit(EncoderVisitor {}, data);
        }
        return out;
    }
};

} // namespace codec
