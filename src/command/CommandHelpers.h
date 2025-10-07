#pragma once

#include "Codec.h"
#include <string>

namespace command {
std::string toUpper(const std::string& str);
std::string extractBulkString(const codec::CodecValue& val);
long long parseInteger(const std::string& str);
double parseDouble(const std::string& str);
} // namespace command
