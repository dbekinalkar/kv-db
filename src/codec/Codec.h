#pragma once

#include "CodecValue.h"
#include <string>

namespace codec {

class Codec {
public:
    static std::string encode(const CodecValue& value);
    static CodecValue decode(const std::string& data);
};
} // namespace codec
